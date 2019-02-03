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
    char OUT_char[100];
    char *Start_point;
    char Str_Cue[23];
    char vejica = ',';
    char stopinja = '"';
    char dvopicje = ':';
    BaseType_t xStatus;
    char * buffer;



    while (1) {
        // Read data from the UART
        int len = uart_read_bytes(UART_NUM_2, data, BUF_SIZE, 200 / portTICK_RATE_MS);

        // len dolzina / data buffer / 
        
        // If there is some data, display it.
        if(len !=0)
        {
            Start_point = strstr( (char * )data, (char *)Find);     // poisce vrstico z $GPLL
            strncpy(OUT_char,Start_point,46);                       // izpise samo 46 charjev
           
            if (OUT_char[44] == 'A'){                               // preveri ce je gps signal veljaven
                
                Str_Cue [0] = OUT_char [7];
                Str_Cue [1] = OUT_char [8];
                Str_Cue [2] = stopinja;
                Str_Cue [3] = OUT_char [9];
                Str_Cue [4] = OUT_char [10];
                Str_Cue [5] = vejica;
                Str_Cue [6] = OUT_char [12];
                Str_Cue [7] = OUT_char [13];
                Str_Cue [8] = dvopicje;
                Str_Cue [9] = OUT_char [18];
                Str_Cue [10] = dvopicje;
                Str_Cue [11] = OUT_char [20];
                Str_Cue [12] = OUT_char [21];
                Str_Cue [13] = OUT_char [22];
                Str_Cue [14] = stopinja;
                Str_Cue [15] = OUT_char [23];
                Str_Cue [16] = OUT_char [24];
                Str_Cue [17] = vejica;
                Str_Cue [18] = OUT_char [26];
                Str_Cue [19] = OUT_char [27];
                Str_Cue [20] = dvopicje;
                Str_Cue [21] = OUT_char [32];
                Str_Cue [22] = '\0';
            }
            else
            { 
                //Str_Cue = "NO GPS";
                strcpy(Str_Cue, "NO GPS");

            }           
            buffer = &Str_Cue;  
            xStatus = xQueueSend( gps_queue, ( void * ) &buffer, 0 );  // send string to cue
            
            if( xStatus != pdPASS )
            {
                /* The send operation could not complete because the queue was full -
                this must be an error as the queue should never contain more than
                one item! */
                printf( "Could not send to the gps_queue.\r\n" );
            }
            memset(data,0, BUF_SIZE);       // Needed to clear data buffer, othwerwise you will see also garbage on output
        }
    }
}