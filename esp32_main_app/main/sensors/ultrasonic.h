#pragma once
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include <string.h>


#define TRIGGER_PIN (GPIO_NUM_33)
#define ECHO_PIN    (GPIO_NUM_35) 
#define THRESHOLD   (20)            // Distance in cm 

extern QueueHandle_t ultrasonic_queue;

void ultrasonic_task(void *pvParameter);