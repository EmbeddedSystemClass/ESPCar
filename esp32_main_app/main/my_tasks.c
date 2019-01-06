#include "my_tasks.h"


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

void ultrasonic_task(void *pvParameter)
{
    /*Trigger pin setup*/
    gpio_pad_select_gpio(TRIGGER_PIN);
    gpio_set_direction(TRIGGER_PIN, GPIO_MODE_OUTPUT);
    
    /*Echo pin setup*/
    gpio_pad_select_gpio(ECHO_PIN);
    gpio_set_direction(ECHO_PIN, GPIO_MODE_INPUT);
    gpio_pulldown_en(ECHO_PIN);

    /*Needed variables*/
    int64_t startOfPulse = 0;
    int64_t endOfPulse = 0;
    float distance = 0;
    float divider = 58;
    
    while(1)
    {
        /*Send short pulse*/
        gpio_set_level(TRIGGER_PIN, 1);
        vTaskDelay(1 / portTICK_PERIOD_MS);
        gpio_set_level(TRIGGER_PIN, 0);
        
        /*Get the time at the start of the pulse*/
        while(gpio_get_level(ECHO_PIN) == 0){}
        startOfPulse = esp_timer_get_time(); 

        /*Get the time at the end of the pulse*/
        while(gpio_get_level(ECHO_PIN) == 1){}
        endOfPulse = esp_timer_get_time(); 

        /*Calculate the distance to 1 mm accuracy*/
        distance = (float) (endOfPulse - startOfPulse) / divider;
        printf("%0.1f cm.\n", distance);
        vTaskDelay(100 / portTICK_PERIOD_MS);           // Change time here to change sample time
    }   
}

void gps_task(void *pvParameter)
{
    /* Uart parameters*/
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    /*Configure paramters, set pins, install the driver*/
    uart_param_config(UART_NUM_2, &uart_config);
    uart_set_pin(UART_NUM_2, ECHO_TEST_TXD, ECHO_TEST_RXD, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_2, BUF_SIZE * 2, 0, 0, NULL, 0);

    /*Allocate buffer*/
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    while (1) {
        // Read data from the UART
        int len = uart_read_bytes(UART_NUM_2, data, BUF_SIZE, 200 / portTICK_RATE_MS);
        
        // If there is some data, display it.
        if(len !=0)
        {
            printf("%s", data);             // Display on monitor terminal 
            printf("\n");                   // For clarity in monitor terminal
            memset(data,0, BUF_SIZE);       // Needed to clear all data buffer, othwerwise you will see also garbage on output
        }
    }
}