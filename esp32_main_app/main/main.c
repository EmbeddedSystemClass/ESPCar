/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/uart.h"



#include "my_tasks.h"
#include "wifi.h"


void app_main()
{
    xTaskCreate(&white_task, "white_task", 2048, NULL, 4, NULL);
    //xTaskCreate(&ultrasonic_task, "ultrasonic_task", 2048, NULL, 3, NULL);
    //xTaskCreate(&gps_task, "gps_task", 2048, NULL, 3, NULL);

}