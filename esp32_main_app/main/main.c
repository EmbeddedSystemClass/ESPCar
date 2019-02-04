/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "nvs_flash.h"

#include "sensors/gps.c" 
#include "sensors/ultrasonic.c" 
//#include "sensors/accelmagneto.h" 
#include "comms/tcpconn.c"
#include "actuators/motorcontrol.c"

#define GREEN_BLINK (GPIO_NUM_2)

void green_task(void *pvParameter)
{
    /*White Led setup*/
    gpio_pad_select_gpio(GREEN_BLINK);
    gpio_set_direction(GREEN_BLINK, GPIO_MODE_OUTPUT);
    const TickType_t xDelay1000ms = pdMS_TO_TICKS( 1000 );
    while(1) {
         /* Blink off (output low) */
        gpio_set_level(GREEN_BLINK, 1);
        vTaskDelay(xDelay1000ms);
        /* Blink on (output high) */
        gpio_set_level(GREEN_BLINK, 0);
        vTaskDelay(xDelay1000ms);   
    }
}

QueueHandle_t motor_queue;
QueueHandle_t ultrasonic_queue;
QueueHandle_t gps_queue;

void app_main()
{
	// Initialize non-volatile flash space
	 nvs_flash_init();

    //Start up the event handler 
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_event_group = xEventGroupCreate();

    //Setup IP of ESP32 and start DHCP 
    start_dhcp_server();

    // Set ESP32 as AP
    initialise_wifi_in_ap();

    motor_queue = xQueueCreate( 20, sizeof( char* ) );
    if( motor_queue == 0 ) printf("Failed to create motor_queue.\n");

    ultrasonic_queue = xQueueCreate( 1, sizeof( int* ) );
    if( ultrasonic_queue == 0 ) printf("Failed to create ultrasonic_queue.\n");

    gps_queue = xQueueCreate( 20, sizeof( char* ) );
    if( gps_queue == 0 ) printf("Failed to create gps_queue.\n");

    xTaskCreate(&green_task, "green_task", 1024, NULL, 1, NULL);
    xTaskCreate(&ultrasonic_task, "ultrasonic_task", 2048, NULL, 1, NULL);
    xTaskCreate(&gps_task, "gps_task", 2048, NULL, 1, NULL);

	xTaskCreate(&tcp_server_task,"tcp_server",4096,NULL,5,NULL);
    //xTaskCreate(&print_sta_info,"print_sta_info",4096,NULL,5,NULL);
    xTaskCreate(&motor_control_task, "motor_control_task", 4096, NULL, 5, NULL);

    // // Initialize accelmagneto sensor
    // IMU_Init();
}