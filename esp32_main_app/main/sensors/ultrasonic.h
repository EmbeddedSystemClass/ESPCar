#pragma once
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include <string.h>


#define TRIGGER_PIN (19)
#define ECHO_PIN    (22) 

void ultrasonic_task();