/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"

#include "sensors/gps.c" 
#include "sensors/ultrasonic.c" 
#include "sensors/accelmagneto.c" 
#include "comms/tcpconn.c"
#include "actuators/motorcontrol.c"

#define WHITE_BLINK (GPIO_NUM_18)

void white_task(void *pvParameter)
{
    /*White Led setup*/
    gpio_pad_select_gpio(WHITE_BLINK);
    gpio_set_direction(WHITE_BLINK, GPIO_MODE_OUTPUT);

    while(1) {
         /* Blink off (output low) */
        gpio_set_level(WHITE_BLINK, 1);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        gpio_set_level(WHITE_BLINK, 0);
        vTaskDelay(100 / portTICK_PERIOD_MS);   
    }
}

void app_main()
{
		// Initialize non-volatile flash space
	nvs_flash_init();

    // Start up the event handler 
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_event_group = xEventGroupCreate();

    // Setup IP of ESP32 and start DHCP 
    start_dhcp_server();

    // Set ESP32 as AP
    initialise_wifi_in_ap();

	gpio_pad_select_gpio(WHITE_BLINK);
    gpio_set_direction(WHITE_BLINK, GPIO_MODE_OUTPUT);

    //xTaskCreate(&white_task, "white_task", 2048, NULL, 4, NULL);
    //xTaskCreate(&ultrasonic_task, "ultrasonic_task", 2048, NULL, 3, NULL);
    xTaskCreate(&gps_task, "gps_task", 2048, NULL, 3, NULL);

	//xTaskCreate(&tcp_server_task,"tcp_server",4096,NULL,5,NULL);
    //xTaskCreate(&print_sta_info,"print_sta_info",4096,NULL,5,NULL);
}