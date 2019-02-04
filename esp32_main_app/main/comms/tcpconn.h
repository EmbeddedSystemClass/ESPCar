#pragma once 
#include <stdio.h>
#include <string.h>
#include <sys/fcntl.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "driver/gpio.h"


#define WHITE_BLINK (GPIO_NUM_16)
#define AP_SSID "ESP_32"
#define AP_PASSPHARSE "12345678"            


//Static variables for following states of tasks read_message_task and send_message_task
static int read_message_running = 0;
static int send_message_running = 0;


extern QueueHandle_t motor_queue;
extern QueueHandle_t gps_queue;

// Used for wifi event handling
static EventGroupHandle_t wifi_event_group;
// const int CLIENT_CONNECTED_BIT = BIT0;
// const int CLIENT_DISCONNECTED_BIT = BIT1;
// const int AP_STARTED_BIT = BIT2;
#define CLIENT_CONNECTED_BIT BIT0
#define CLIENT_DISCONNECTED_BIT  BIT1
#define AP_STARTED_BIT BIT2

// Used for log ouput
static const char *TAG="ap_mode_tcp_server";

/* Function is looking for specific wifi related events, such as 
connection of station, start of Ap point etc. If event happens 
it sets appropriate bit in wifi_event_group, thus enabling execution 
of code that was dependend on event. */
esp_err_t event_handler(void *ctx, system_event_t *event);

/* Function assings static IP to ESP32 and starts up DHCP server*/
void start_dhcp_server();

/* Creates an Access Point*/
void initialise_wifi_in_ap();

/* Print the list of connected stations */
static void printStationList();


void print_sta_info(void *pvParameters); 

 
/* Function starts up a TCP connection: it creates a socket, 
binds it and listens for new connections. After it accepts 
the new connection, it will turn on and off an led, which is 
used for testing purposes.*/
void tcp_server_task(void *pvParameters);

void read_message_task(void *pvParameters);

void send_message_task(void *pvParameters);