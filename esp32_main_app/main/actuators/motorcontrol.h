#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_attr.h"

#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"


#define AIN1 (GPIO_NUM_12)  // Right side forward
#define AIN2 (GPIO_NUM_14)  // Right side backward
#define BIN2 (GPIO_NUM_26)  // Left side forward
#define BIN1 (GPIO_NUM_25)  // left side backward
#define SLP  (GPIO_NUM_27)  // Sleep pin, pull high to enable H-Bridge

#define SPEED (70.0F)

#define TURN (100.0F)

extern QueueHandle_t motor_queue;
extern QueueHandle_t ultrasonic_queue;

void motor_control_task(void *pvParameter);

void enable_hbridge(uint8_t enable);

void drive_forward(float speed);

void drive_backward(float speed);

void turn_right(float speed);

void turn_left(float speed);

void drive_forward_right(float speed);

void drive_forward_left(float speed);

void stop();



