#include "ultrasonic.h"

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
    
    /*Variables for queue*/
    BaseType_t xStatus;
    int * buffer;
    int stopFlag = 0; 

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
        //printf("%0.1f cm.\n", distance);

        if (distance < THRESHOLD) 
        {
            stopFlag = 1; 
            printf("stopFlag = 1\n");
        }
        else 
        {   stopFlag = 0;
        }
        buffer = &stopFlag;
        /*Send to queue*/
        xStatus = xQueueSend( ultrasonic_queue, ( void * ) &buffer, 0 );

        if( xStatus != pdPASS )
        {
            printf( "Could not send to the motor_queue.\r\n" );
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);           // Change time here to change sample time
    }   
}