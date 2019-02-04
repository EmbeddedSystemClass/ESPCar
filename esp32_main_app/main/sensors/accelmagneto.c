#include "accelmagneto.h"
#include "accelmagneto_private.h"

//#include "freertos/FreeRTOSConfig.h"

#include "driver/i2c.h"

#define IMU_ADDRESS 0x1E

#define FXOS8700_ID                (0xC7)

#define ACK_CHECK_EN 0x1            /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0           /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                 /*!< I2C ack value */
#define NACK_VAL 0x1 /*!< I2C nack value */

static struct imu imu_dev;

static i2c_port_t i2c_port = I2C_NUM_0;

static uint8_t i2c_read_byte(uint8_t reg);
static void i2c_write(uint8_t reg, uint8_t val);
static void i2c_read(uint8_t reg, uint8_t size, uint8_t* data);
static void IMU_Task(void* arg);

imu_Handle_t imu_handle = NULL;

static void IMU_GetData(imu_data_t* data)
{
    uint8_t rawData[13];
    i2c_read(FXOS8700_REGISTER_STATUS | 0x80, 13, rawData);

    //printf("%i\n", (int16_t)rawData);
    data->accel.x = (int16_t)((rawData[1] << 8) | rawData[2]) >> 2;
    data->accel.y = (int16_t)((rawData[3] << 8) | rawData[4]) >> 2;
    data->accel.z = (int16_t)((rawData[5] << 8) | rawData[6]) >> 2;
    data->mag.x = (int16_t)((rawData[7] << 8) | rawData[8]);
    data->mag.y = (int16_t)((rawData[9] << 8) | rawData[10]);
    data->mag.z = (int16_t)((rawData[11] << 8) | rawData[12]);

    /* Convert accel values to m/s^2 */
    #if 0
    #define ACCEL_MG_LSB_2G (0.000244F)
    #define SENSORS_GRAVITY_STANDARD (9.80665F)
    accelEvent->acceleration.x *= ACCEL_MG_LSB_2G * SENSORS_GRAVITY_STANDARD;
    accelEvent->acceleration.y *= ACCEL_MG_LSB_2G * SENSORS_GRAVITY_STANDARD;
    accelEvent->acceleration.z *= ACCEL_MG_LSB_2G * SENSORS_GRAVITY_STANDARD;
    #endif
}


static uint8_t I2C_init()
{
    esp_err_t err_code;
    i2c_config_t config;
    config.mode = I2C_MODE_MASTER;
    config.scl_io_num = IMU_SCL_PIN;
    config.sda_io_num = IMU_SDA_PIN;
    config.scl_pullup_en = GPIO_PULLUP_DISABLE;
    config.sda_pullup_en = GPIO_PULLUP_DISABLE;
    config.master.clk_speed = 400000;
    err_code = i2c_param_config(i2c_port, &config);
    if(err_code != ESP_OK)
        return 1;
    err_code = i2c_driver_install(i2c_port, I2C_MODE_MASTER, 0, 0, 0);
    if(err_code != ESP_OK)
        return 1;

    return 0;
}

static void imu_low_level_Init()
{
    //Check that IC is properly connected
    uint8_t id = i2c_read_byte(FXOS8700_REGISTER_WHO_AM_I);
    if(id != FXOS8700_ID)
    {
        //Error
    }

      /* Set to standby mode (required to make changes to this register) */
    i2c_write(FXOS8700_REGISTER_CTRL_REG1, 0);
    //Set range to 2G
    i2c_write(FXOS8700_REGISTER_XYZ_DATA_CFG, 0x00);

    /* High resolution */
    i2c_write(FXOS8700_REGISTER_CTRL_REG2, 0x02);
    /* Active, Normal Mode, Low Noise, 100Hz in Hybrid Mode */
    i2c_write(FXOS8700_REGISTER_CTRL_REG1, 0x15);

    /* Configure the magnetometer */
    /* Hybrid Mode, Over Sampling Rate = 16 */
    i2c_write(FXOS8700_REGISTER_MCTRL_REG1, 0x1F);
    /* Jump to reg 0x33 after reading 0x06 */
    i2c_write(FXOS8700_REGISTER_MCTRL_REG2, 0x20);
}

static uint8_t i2c_read_byte(uint8_t reg)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    uint8_t addr = (IMU_ADDRESS << 1);

    uint8_t data;
    //i2c_read(reg, 1, &data);

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, addr, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, addr | I2C_MASTER_READ, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, &data, ACK_CHECK_DIS);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_port, cmd, 50/portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return data;
}  

static void i2c_write(uint8_t reg, uint8_t val)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    uint8_t address = (IMU_ADDRESS << 1);
    i2c_master_start(cmd);
    //todo
    i2c_master_write_byte(cmd, address, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, val, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_port, cmd, 50/portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
}

static void i2c_read(uint8_t reg, uint8_t size, uint8_t* data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    uint8_t addr = (IMU_ADDRESS << 1);

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, addr, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, addr | I2C_MASTER_READ, ACK_CHECK_EN);
    i2c_master_read(cmd, data, size - 2, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, &(data[size - 1]), ACK_CHECK_DIS);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_port, cmd, 50/portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    printf("%i\n", ret);
}

uint8_t imu_low_level_init()
{
    // Initialize I2C
    I2C_init();
    imu_low_level_Init();
    return 0;
}

void IMU_Init()
{
    xTaskCreate(IMU_Task, "IMU Task", 4096, NULL, 1, NULL);
}

static void IMU_Task(void* arg)
{
    imu_handle = &imu_dev;

    imu_handle->q = xQueueCreate(1, sizeof(imu_data_t));

    if(imu_handle->q == NULL){
        //error
    }
    
    //Initialize hardware
    if(imu_low_level_init() > 0){
        //Error occoured, cleanup and return NULL
        vQueueDelete(imu_handle->q);
        //Error
    }
    //return h;

    imu_data_t imu_data;

    while(1){
        IMU_GetData(&imu_data);
        printf("%i\n", imu_data.accel.x);
        printf("%i\n", imu_data.accel.y);
        printf("%i\n", imu_data.accel.z);
        //xQueueSendToBack(imu_handle->q, &imu_data, 0);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}