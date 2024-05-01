/*
 * RFID.h
 *
 * Created: 14/02/2024 3:29:52 CH
 *  Author: Vũ Huy Bảo
 */ 


#ifndef RFID_H_
#define RFID_H_

#include "spi_module.h"



#define     MAX_LEN               16        // Maximum length of an array.

// MF522 MFRC522 error codes.
#define     mi_ok                    0         // Everything A-OK.
#define     mi_err                   1         // General error
#define     mi_notag_err			 2         // there is no tag for communication

#define sbi(port,bit) {port|= (1<<bit);}
#define cbi(port,bit) {port&= (~(1<<bit));}
#define none_value 0x00

//////////////////////COMMAND WORD////////////////////////////
#define mfrc522_idle              0x00  // no action, cancels current command execution
#define mfrc522_mem               0x01  // store 25 bytes into the internal buffer
#define mfrc522_generaterandomid  0x02  // generates a 10 bytes random ID number
#define mfrc522_calcrc            0x03
#define mfrc522_transmit          0x04  // transmit data
#define mfrc522_nocmdchange       0x07  // no command change
#define mfrc522_receive           0x08  // receive data
#define mfrc522_transceive        0x0C
#define mfrc522_mf_authent        0x0E 
#define mfrc522_softreset         0x0F    
//-----------------------------------------------//


/////////////////////////MIFARE_ONE TAG COMMON word/////////////////////////

#define mf1_reqa        0x26 // request
#define mf1_wupa        0x52 // wake up
#define mf1_anticol_1   0x93 // anti collision CL1
#define mf1_select_1    0x93 // select tag CL1
#define mf1_anticol_2   0x95 // anti collision CL2
#define mf1_select_2    0x95 // select tag CL2
#define mf1_halt        0x50 // halt the process
#define mf1_authenA     0x60 // authentication with key A
#define mf1_authenB     0x61 // authentication with key B
#define mf1_read        0x30 // read
#define mf1_write       0xA0 // write
#define mf1_decrement   0xC0 // decrement the content of the block and the res is store in internal transfer buffer
#define mf1_increment   0xC1 // increment the content of the block and the res is store in internal transfer buffer
#define mf1_restore     0xC2 // move the content of block to internal transfer buffer
#define mf1_transfer    0xB0 // move the content of internal transfer buffer to block

///////////////// MFRC522 REGISTER //////////////////////
// Page 0: command and status
#define     Reserved00            0x00
#define     CommandReg            0x01
#define     CommIEnReg            0x02
#define     DivIEnReg             0x03
#define     CommIrqReg            0x04
#define     DivIrqReg             0x05
#define     ErrorReg              0x06
#define     Status1Reg            0x07
#define     Status2Reg            0x08
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
#define     Reserved01            0x0F
//Page 1:Command
#define     Reserved10            0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxASKReg              0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
#define     Reserved11            0x1A
#define     Reserved12            0x1B
#define     MifareReg             0x1C
#define     Reserved13            0x1D
#define     Reserved14            0x1E
#define     SerialSpeedReg        0x1F
//Page 2:CFG
#define     Reserved20            0x20
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
#define     Reserved21            0x23
#define     ModWidthReg           0x24
#define     Reserved22            0x25
#define     RFCfgReg              0x26
#define     GsNReg                0x27
#define     CWGsPReg              0x28
#define     ModGsPReg             0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C 
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F
//Page 3:TestRegister
#define     Reserved30            0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39
#define     TestDAC2Reg           0x3A
#define     TestADCReg            0x3B
#define     Reserved31            0x3C
#define     Reserved32            0x3D
#define     Reserved33            0x3E
#define     Reserved34            0x3F
//-----------------------------------------------//



class mfrc522 {
	private: 
		uint8_t ss_pin;
		uint8_t rst;
	public:
		mfrc522             (int ss_pin, int reset);
		void    write_to_reg(uint8_t addr, uint8_t data);
		uint8_t read_from_reg(uint8_t addr);
		void    setbitmask(uint8_t address, unsigned char mask);
		void    clearbitmask(uint8_t address, unsigned char mask); 
		void    reset();
		void    RFID_init();
		uint8_t getfirmwareversion();
		int     commandtag(uint8_t cmd, uint8_t *data, int dlen, uint8_t *res, int* rlen);
		void    calculateCRC(uint8_t *data, int len, uint8_t *result);
		uint8_t selecttag(uint8_t* serial);
		int     requestag(uint8_t* data);
		int     halttag();
		int     anticollsion(uint8_t* serial);
	};



#endif /* RFID_H_ */