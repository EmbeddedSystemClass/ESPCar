
#ifndef _IMU_H_
#define _IMU_H_

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

//TODO: move to board header file
#define IMU_SDA_PIN 18
#define IMU_SCL_PIN 19

typedef enum
{
    IMU_STATUS_UNINITIALIZED,
    IMU_STATUS_READY,
    IMU_STATUS_BUSY
} imu_status_t;

struct imu
{
    QueueHandle_t q;
};

struct imu_accel
{
    int16_t x;
    int16_t y;
    int16_t z;
};

struct imu_magnetometer
{
    int16_t x;
    int16_t y;
    int16_t z;
};

typedef struct
{
    struct imu_accel accel;
    struct imu_magnetometer mag;
} imu_data_t;

typedef struct imu* imu_Handle_t;

extern imu_Handle_t imu_handle;

void IMU_Init();

#define IMU_READ_PERIOD_MS 100

#endif