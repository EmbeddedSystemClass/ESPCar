#pragma once
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include <string.h>

#define WHITE_BLINK (18)
#define TRIGGER_PIN (19)
#define ECHO_PIN    (22) 

#define ECHO_TEST_TXD  (GPIO_NUM_17)
#define ECHO_TEST_RXD  (GPIO_NUM_16)
#define BUF_SIZE (2048)


void white_task();
void ultrasonic_task();
void gps_task();
