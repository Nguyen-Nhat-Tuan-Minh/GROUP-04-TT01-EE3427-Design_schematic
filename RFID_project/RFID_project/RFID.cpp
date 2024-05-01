/*
 * CPPFile1.cpp
 *
 * Created: 14/02/2024 3:29:00 CH
 *  Author: Vũ Huy Bảo
 */ 

/*

data frame between MFRC522 and Mifare card s50

+ All given times refer to the data frames including start of communication and end of
communication. A PCD data frame contains the start of communication (1 "start bit")
and the end of communication (one logic 0 + 1 bit length of unmodulated carrier). A
PICC data frame contains the start of communication (1 "start bit") and the end of
communication (1 bit length of no subcarrier)

+ Max DATA length is 128 bit (16 bytes) (not include CRC biit or parity bit)

+ send 8 bit each

+ have 16bits CRC start from request_cmd


*/


#include "RFID.h"



// initial function
mfrc522::mfrc522(int ss_pin, int reset){
    this->ss_pin = ss_pin;
    (sbi(PORTB, this->ss_pin));
    (sbi(DDRB, this->ss_pin));
    
    this->rst = reset;
    (sbi(PORTB, this->rst));
    (sbi(DDRB, this->rst));
}


// function: write to reg of MFRC522
void mfrc522::write_to_reg(uint8_t addr, uint8_t data){
    (cbi(PORTB, this->ss_pin));
    
    spi_transmit((addr<<1) & 0x7E);
    spi_transmit(data);
    
    (sbi(PORTB, this->ss_pin));
    
}

// function: read from reg of MFRC522
uint8_t mfrc522::read_from_reg(uint8_t addr){
    (cbi(PORTB, this->ss_pin));
    
    uint8_t return_data;
    spi_transmit(((addr<<1) & 0x7E) | 0x80);
    return_data = spi_transmit(none_value);
    
    (sbi(PORTB, this->ss_pin));
    
    return return_data;
}

// function: set a bit from reg of MFRC522
void mfrc522::setbitmask(uint8_t address, unsigned char mask){
    uint8_t temp = 0x00;
    temp = read_from_reg(address);
    write_to_reg(address, temp | mask);
}
    
// function: clear a bit from reg of MFRC522   
void mfrc522::clearbitmask(uint8_t address, unsigned char mask){
    uint8_t temp = 0x00;
    temp = read_from_reg(address);
    write_to_reg(address, temp & (~mask));
}

// function: send a soft reset cmd to MFRC522
void mfrc522::reset(){
    write_to_reg(CommandReg, mfrc522_softreset);
}

uint8_t mfrc522::getfirmwareversion(){
    uint8_t response = 0 ;
    response = read_from_reg(VersionReg);
    return response;
}

// calculate 16bit CRC and read it
void mfrc522::calculateCRC(uint8_t *data, int len, uint8_t *result) {
    int i;
    uint8_t n;

    clearbitmask(DivIrqReg, 0x04);   // clear crc interrupt flag:CRC command is active and all bits are processed
    setbitmask(FIFOLevelReg, 0x80);  // clear read and write pointer

    // Writing data to the FIFO.
    for (i = 0; i < len; i++) {
        write_to_reg(FIFODataReg, data[i]);
    }
    write_to_reg(CommandReg, mfrc522_calcrc);

    // Wait for the CRC calculation to complete.
    i = 0xFF;
    do {
        n = read_from_reg(DivIrqReg);
        i--;
    } while ((i != 0) && !(n & 0x04));  // CRCIrq = 1

    // Read the result from the CRC calculation.
    result[0] = read_from_reg(CRCResultRegL);
    result[1] = read_from_reg(CRCResultRegM);
}


// function: send cmd for initializing RFID
void mfrc522::RFID_init(){
    sbi (PORTB, this->ss_pin);
    reset();
    // timer: 3390*2*48/13.56MHz = 24ms (for each cmd the timer will run for the next cmd)
    // prescaler
    write_to_reg(TModeReg, 0x8D);
    write_to_reg(TPrescalerReg, 0x3E);
    
    // reload_counter
	write_to_reg(TReloadRegL, 0x30);
    write_to_reg(TReloadRegH, 0x00);

    // modulation
    write_to_reg(TxASKReg, 0x40); // 100% ASK (amplitude shift keying)
    write_to_reg(TModeReg, 0x3D);  // transmit only when an RF field is generated
    setbitmask(TxControlReg, 0x03); // allow RF signal to transmit on Tx1 and Tx2 pins
}

// this function for writing cmd to mifare card
int mfrc522::commandtag(uint8_t cmd, uint8_t *data, int dlen, uint8_t *res, int* rlen){
    int status = mi_err;
    uint8_t irqen = 0x00;
    uint8_t wait_irq = 0x00;
    uint8_t lastbits, n ;
    int i ;

    switch(cmd){
        case mfrc522_mf_authent:
            irqen = 0x12; // enable idle cmd and error protocol interrupt to connect irq pin
            wait_irq = 0x10;
            break;
        case mfrc522_transceive:
            irqen = 0x77; // enable idle cmd, error protocol, transmit, receive, lower alert fifo and timer counter value gets zero interrupt to connect irq pin
            wait_irq = 0x30;
            break;
        default:
            break;
    }

    write_to_reg(CommIEnReg, irqen | 0x80); // interrupt request
    clearbitmask(CommIrqReg, 0x80); // clear all interrupt request bits (if it's already set from the beginning)
    setbitmask(FIFOLevelReg, 0x80); // clear read and write pointer if FIFO

    write_to_reg(CommandReg, mfrc522_idle); // no action, cancel the current command

    // write to fifo 
    for (i = 0; i < dlen; i++){
        write_to_reg(FIFODataReg, data[i]);
    }


    // execute the command
    write_to_reg(CommandReg, cmd);

    if (cmd == mfrc522_transceive){
        setbitmask(BitFramingReg, 0x80); // start the transmission and read continuously between tag and reader (data is get from FIFO)
        // the last byte transmit is 8 bit
    }

    // waiting for the command complete, so that we can receive data

    i = 25; // max wait time is 125ms
    do {
        _delay_ms(5);
        // CommIRqReg[7..0]
        // Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
        n = read_from_reg(CommIrqReg);
        i--;
    } while ((i != 0) && !(n & 0x01) && !(n & wait_irq)); // wait for timer, idle interrupt, (and receive flag interrupt for transceive)

    clearbitmask(BitFramingReg, 0x80); // stop transceive

    if (i != 0) { // request did not timeout
        if (!(read_from_reg(ErrorReg) & 0x1D)) {    // BufferOvfl Collerr CRCErr ProtocolErr
            status = mi_ok;
            if (n & irqen & 0x01) {
                status = mi_notag_err;
            }
            if (cmd == mfrc522_transceive){
                n = read_from_reg(FIFOLevelReg);
                lastbits = read_from_reg(ControlReg) & 0x07;
                if (lastbits){
                    *rlen = (n - 1) * 8 + lastbits; // indicate that the number of bits of the last byte can be different 
                } else {
                    *rlen = n * 8;
                }
            }

            if (n == 0) {
                n = 1;
            }
            if (n > MAX_LEN) {
                n = MAX_LEN; // 106KB per second : 128 bits (16 bytes) therefore can communicate less than 16 bytes
            }

            // reading the receive data from FIFO
            for (i = 0; i < n; i++){
                res[i] = read_from_reg(FIFODataReg);
            }
        } 
		else status = mi_err;
    } 
    return status;
}

// just only for requesting mf1 tag
int mfrc522::requestag(uint8_t* data){
    int status; 
    int len;
    write_to_reg(BitFramingReg, 0x07); // 7 bits for request cmd
    data[0] = mf1_reqa;
    status = commandtag(mfrc522_transceive, data, 1, data, &len);
    if ((status != mi_ok) || (len != 0x10)) { // 16-bit ATQA response from tag
        status = mi_err;
    }

    return status;
}

int mfrc522::anticollsion(uint8_t* serial){
    int status, len;
    write_to_reg(BitFramingReg, 0x00);
    serial[0] = mf1_anticol_1;
    serial[1] = 0x20;
    status = commandtag(mfrc522_transceive, serial, 2, serial, &len);
    len = len / 8; //(unit: bytes )
    if ((len == 4)&&(status == mi_ok)) {
        status = mi_ok;
    }
    return status; 
}


uint8_t mfrc522::selecttag(uint8_t* serial){
    int i, status, len;
    uint8_t ack = 0x08 ;
    uint8_t buffer[8];
    
    write_to_reg(BitFramingReg, 0x00);
    
    buffer[0] = mf1_select_1;
    buffer[1] = 0x70;
    for (i = 0; i < 4; i++){
        buffer[i + 2] = serial[i];
    }
    calculateCRC(buffer, 6, &buffer[6]);
    
    status = commandtag(mfrc522_transceive, buffer, 8, buffer, &len);
    if ((status == mi_ok) && (buffer[0] == ack)) {
        status = mi_ok;
    }
    return status;
}



int mfrc522::halttag(){
    int status, len;
    uint8_t buffer[4];
    
    write_to_reg(BitFramingReg, 0x00);
    
    buffer[0] = mf1_halt;
    buffer[1] = 0;
    calculateCRC(buffer, 2, &buffer[2]);
    status = commandtag(mfrc522_transceive, buffer, 4, buffer, &len);
    clearbitmask(Status2Reg, 0x08); // turn off encryption
    return status;
}
