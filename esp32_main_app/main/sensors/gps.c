#include "gps.h"

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
    const uint8_t Find[] = "$GPGLL";
    char OUT_char[50];
    char *Start_point;
    int  Read_point;


    while (1) {
        // Read data from the UART
        int len = uart_read_bytes(UART_NUM_2, data, BUF_SIZE, 200 / portTICK_RATE_MS);

        // len dolzina / data buffer / 
        
        // If there is some data, display it.
        if(len !=0)
        {
            Start_point = strstr( (char * )data, (char *)Find);
            strncpy(OUT_char,Start_point,46);
            if (OUT_char[44] == A){
                printf("%s", OUT_char);             // Display on monitor terminal 
                printf("\n");
            }else{
                printf("NO GPS");                   // If no signal display no GPS
                printf("\n"); 
            }
        
            memset(data,0, BUF_SIZE);       // Needed to clear all data buffer, othwerwise you will see also garbage on output
        }
    }
}