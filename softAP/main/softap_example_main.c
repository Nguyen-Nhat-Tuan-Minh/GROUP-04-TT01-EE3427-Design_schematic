/*  WiFi softAP Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include <stdbool.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/param.h>
#include "esp_netif.h"
#include <esp_http_server.h>
//#include "esp_event.h"
//#include "esp_netif.h"
#include "esp_tls.h"

#include "driver/gpio.h"
#include <driver/uart.h>
#include <soc/uart_struct.h>

#if !CONFIG_IDF_TARGET_LINUX
#include <esp_wifi.h>
#include <esp_system.h>
#include "nvs_flash.h"
#endif  // !CONFIG_IDF_TARGET_LINUX

#define EXAMPLE_HTTP_QUERY_KEY_MAX_LEN  (64)

static const char *TAG = "webserver";
const char *predetermined_values[] = {"12 34 45 43", "56 78 34 56", "AB CD 43 21", "EF GH 34 75"};

#define LED GPIO_NUM_2
#define CARD_UID_LENGTH 12 // Assuming RFID card UID is of 4 bytes length
char card_uid[CARD_UID_LENGTH];

/**************WEBSERVER CODING SECTION LMAO**************/

// Function to check if card UID matches any predetermined values
bool isCardValid(const char *card_uid, const char **predetermined_values, int num_values) {
    // Iterate through predetermined values array
    for (int i = 0; i < num_values; i++) {
        // Compare card UID with predetermined value
        if (strncmp(card_uid, predetermined_values[i], CARD_UID_LENGTH) == 0) {
            // Card UID matches, return true
            return true;
        }
    }
    // Card UID does not match any predetermined value, return false
    return false;
}



static esp_err_t card_handler(httpd_req_t *req)
{
    esp_err_t error;
    
    if (isCardValid(card_uid, predetermined_values, 4))
    {
        ESP_LOGI(TAG, "Card Valid!"); // send log RFID card valid
        gpio_set_level(LED, 1);
    }
    else
    {
        ESP_LOGI(TAG, "Card Invalid!"); // send log RFID card invalid
        gpio_set_level(LED, 0);
    }

    //ESP_LOGI(TAG, "Card Invalid!"); // send log RFID card invalid
    //gpio_set_level(LED, 0);
    const char *response; //= (const char *) req->user_ctx; // copy user ctx into response
    if (isCardValid(card_uid, predetermined_values, 4)) 
    {
        response = "<!DOCTYPE html>\
<html>\
<head>\
<style>\
body {\
  font-family: 'Roboto', sans-serif; /* Google font */\
}\
\
.infographic {\
  border: 1px solid #0077B6; /* Border color */\
  padding: 10px;\
  margin-bottom: 20px;\
  border-radius: 10px; /* Smoother corners */\
  background-color: #E5F6F8; /* Background color */\
}\
\
.card-status {\
  border: 1px solid #08A045; /* Green border color */\
  padding: 10px;\
  margin-top: 20px;\
  border-radius: 10px; /* Smoother corners */\
  background-color: #08A045; /* Green background color */\
  text-align: center; /* Center align text */\
  color: #FFFFFF; /* White text color */\
  display: flex; /* Use flexbox for centering */\
  justify-content: center; /* Center horizontally */\
  align-items: center; /* Center vertically */\
}\
\
.card-status h3 {\
  margin: 0; /* Remove default margin */\
}\
\
.card-uid {\
  font-weight: bold; /* Bold font */\
}\
\
h1 {\
  text-align: center;\
  color: #0077B6;\
}\
\
h3 {\
  text-align: left; /* Left align the text */\
}\
</style>\
<link href=\"https://fonts.googleapis.com/css2?family=Roboto:wght@400;700&display=swap\" rel=\"stylesheet\">\
</head>\
<body>\
\
<h1>RFID CARD READER WEB SERVER</h1>\
<div class=\"infographic\">\
  <p class=\"card-uid\">Card UID: %s</p>\
  <h3>Card state: Read</h3>\
</div>\
\
<div class=\"card-status\">\
  <h3>Card Valid</h3>\
</div>\
\
</body>\
</html>"
;
    } 
    else 
    {
        response = "<!DOCTYPE html>\
<html>\
<head>\
<style>\
body {\
  font-family: 'Roboto', sans-serif; /* Google font */\
}\
\
.infographic {\
  border: 1px solid #0077B6; /* Border color */\
  padding: 10px;\
  margin-bottom: 20px;\
  border-radius: 10px; /* Smoother corners */\
  background-color: #E5F6F8; /* Background color */\
}\
\
.card-status {\
  border: 1px solid #D21401; /* Green border color */\
  padding: 10px;\
  margin-top: 20px;\
  border-radius: 10px; /* Smoother corners */\
  background-color: #D21401; /* Green background color */\
  text-align: center; /* Center align text */\
  color: #FFFFFF; /* White text color */\
  display: flex; /* Use flexbox for centering */\
  justify-content: center; /* Center horizontally */\
  align-items: center; /* Center vertically */\
}\
\
.card-status h3 {\
  margin: 0; /* Remove default margin */\
}\
\
.card-uid {\
  font-weight: bold; /* Bold font */\
}\
\
h1 {\
  text-align: center;\
  color: #0077B6;\
}\
\
h3 {\
  text-align: left; /* Left align the text */\
}\
</style>\
<link href=\"https://fonts.googleapis.com/css2?family=Roboto:wght@400;700&display=swap\" rel=\"stylesheet\">\
</head>\
<body>\
\
<h1>RFID CARD READER WEB SERVER</h1>\
<div class=\"infographic\">\
  <p class=\"card-uid\">Card UID: %s</p>\
  <h3>Card state: Read</h3>\
</div>\
\
<div class=\"card-status\">\
  <h3>Card Invalid</h3>\
</div>\
\
</body>\
</html>"
;
    }

    char formatted_response[2000];
    snprintf(formatted_response, sizeof(formatted_response), response, card_uid);

    error = httpd_resp_send(req, formatted_response, strlen(formatted_response)); // server sends response
    if (error != ESP_OK)
    {
        ESP_LOGI(TAG, "Error %d while sending Response", error);
    }
    else ESP_LOGI(TAG, "Response sent Successfully");
    return error;
}

static const httpd_uri_t card = {
    .uri       = "/card",
    .method    = HTTP_GET,
    .handler   = card_handler,
    
};


static esp_err_t root_handler(httpd_req_t *req)
{
    esp_err_t error;
    ESP_LOGI(TAG, "No Card!"); // send log RFID card valid
    gpio_set_level(LED, 1);
    const char *response = (const char *) req->user_ctx; // copy user ctx into response
    error = httpd_resp_send(req, response, strlen(response)); // server sends response
    if (error != ESP_OK)
    {
        ESP_LOGI(TAG, "Error %d while sending Response", error);
    }
    else ESP_LOGI(TAG, "Response sent Successfully");
    return error;
}



static const httpd_uri_t root = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = root_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = "<!DOCTYPE html>\
<html>\
<head>\
<style>\
body {\
  font-family: 'Roboto', sans-serif; /* Google font */\
}\
\
.infographic {\
  border: 1px solid #0077B6; /* Border color */\
  padding: 10px;\
  margin-bottom: 20px;\
  border-radius: 10px; /* Smoother corners */\
  background-color: #E5F6F8; /* Background color */\
}\
\
.author-box {\
  border: 1px solid #0077B6; /* Border color */\
  padding: 10px;\
  margin-top: 20px;\
  border-radius: 10px; /* Smoother corners */\
  background-color: #E5F6F8; /* Background color */\
}\
\
h1 {\
  text-align: center;\
  color: #0077B6;\
}\
</style>\
<link href=\"https://fonts.googleapis.com/css2?family=Roboto:wght@400;700&display=swap\" rel=\"stylesheet\">\
</head>\
<body>\
\
<h1>RFID CARD READER WEB SERVER</h1>\
<div class=\"infographic\">\
  <h3>Card state: Not read</h3>\
</div>\
\
<div class=\"author-box\">\
  <p><strong>Author: Group 06</strong></p>\
</div>\
\
</body>\
</html>\
"
};


esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}


static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
#if CONFIG_IDF_TARGET_LINUX
    // Setting port as 8001 when building for Linux. Port 80 can be used only by a priviliged user in linux.
    // So when a unpriviliged user tries to run the application, it throws bind error and the server is not started.
    // Port 8001 can be used by an unpriviliged user as well. So the application will not throw bind error and the
    // server will be started.
    config.server_port = 8001;
#endif // !CONFIG_IDF_TARGET_LINUX
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &root);
        httpd_register_uri_handler(server, &card);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

#if !CONFIG_IDF_TARGET_LINUX
static esp_err_t stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    return httpd_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        if (stop_webserver(*server) == ESP_OK) {
            *server = NULL;
        } else {
            ESP_LOGE(TAG, "Failed to stop http server");
        }
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}
#endif // !CONFIG_IDF_TARGET_LINUX

/**************WEBSERVER CODING END**************/

static void configure_led(void)
{
    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
}

/* The examples use WiFi configuration that you can set via project configuration menu.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_WIFI_CHANNEL   CONFIG_ESP_WIFI_CHANNEL
#define EXAMPLE_MAX_STA_CONN       CONFIG_ESP_MAX_STA_CONN



static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

void wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .channel = EXAMPLE_ESP_WIFI_CHANNEL,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
#ifdef CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT
            .authmode = WIFI_AUTH_WPA3_PSK,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
#else /* CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT */
            .authmode = WIFI_AUTH_WPA2_PSK,
#endif
            .pmf_cfg = {
                    .required = true,
            },
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS, EXAMPLE_ESP_WIFI_CHANNEL);
}

void app_main(void)
{
    strcpy(card_uid, "12 34 01 FA");
    static httpd_handle_t server = NULL;

    configure_led();
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
    wifi_init_softap();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_AP_STAIPASSIGNED, &connect_handler, &server));
//    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));
}
