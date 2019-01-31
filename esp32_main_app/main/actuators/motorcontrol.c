#include "motorcontrol.h"

void motor_control_task(void *pvParameter)
{
    /* Configure PWM outputs*/
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, AIN1);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, AIN2);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1A, BIN1);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1B, BIN2);
    
    /* Configure two sides using same parameters, using two timers*/
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 1000;                        //frequency = 1000Hz,
    pwm_config.cmpr_a = 0.0;                            //duty cycle of PWMxA = 0
    pwm_config.cmpr_b = 0.0;                            //duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config); 
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config);  

    enable_hbridge(1);

    char * tx_buffer;
    BaseType_t xStatus;
    int * stopFlag;

    while(1)
    {
        xStatus = xQueueReceive( ultrasonic_queue, &( stopFlag ), portMAX_DELAY );
        //printf("Stop flag is: %d\n", *stopFlag);
        xStatus = xQueueReceive( motor_queue, &( tx_buffer ), 0 );
        
        if( xStatus == pdPASS)
        {   
            printf("Received command: %s\n", tx_buffer);
            if(*stopFlag == 0)
            {              
                /* Car can move, no object inside threshold*/
                if(strcmp(tx_buffer, "1") == 0)         drive_forward(SPEED);
                else if(strcmp(tx_buffer, "2") == 0)    drive_backward(SPEED);
                else if(strcmp(tx_buffer, "3") == 0)    turn_left(TURN);
                else if(strcmp(tx_buffer, "4") == 0)    turn_right(TURN);
                else if(strcmp(tx_buffer, "5") == 0)    turn_right(TURN); //drive_forward_right(SPEED);
                else if(strcmp(tx_buffer, "6") == 0)    turn_left(TURN); // drive_forward_left(SPEED);
                else stop();
                memset(tx_buffer,0, 128);
            }
            else
            {
                printf("WE got here");
                /* Object inside threshold, car can only move backward*/
                if(strcmp(tx_buffer, "2") == 0)    
                    drive_backward(SPEED);
                else
                    stop();
                
            }
        }
        else
        {
            if(*stopFlag == 1) 
            {
                if(strcmp(tx_buffer, "2") == 0)    
                    drive_backward(SPEED);
                else
                    stop();
            }
        }

        
        
    }
}



void enable_hbridge(uint8_t enable)
{
    gpio_pad_select_gpio(SLP);
    gpio_set_direction(SLP, GPIO_MODE_OUTPUT);

    if(enable)
        gpio_set_level(SLP, 1);
    else
        gpio_set_level(SLP, 0);    
}

void drive_forward(float speed)
{
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B);     // Stop AIN2
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B);     // Stop BIN1
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, speed);    // Start AIN1
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, speed);    // Start BIN2

    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
}

void drive_backward(float speed)
{
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A);     // Stop AIN1
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A);     // Stop BIN2
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, speed);    // Start AIN2
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B, speed);    // Start BIN1

    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);
}

void turn_right(float speed)
{
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A);     // Stop AIN1
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B);     // Stop BIN1
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, speed);    // Start AIN2
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, speed);    // Start BIN2

    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
}

void turn_left(float speed)
{
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B);     // Stop AIN2
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A);     // Stop BIN2
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, speed);    // Start AIN1
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B, speed);    // Start BIN1
    
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);

}

void stop()
{
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B); // Stop AIN2
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B); // Stop BIN1   
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A); // Stop AIN1
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A); // Stop BIN2
}

void drive_forward_right(float speed)
{
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B);     // Stop AIN2
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B);     // Stop BIN1
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 30.0);    // Start AIN1
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, speed);    // Start BIN2

    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
}

void drive_forward_left(float speed)
{
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B);     // Stop AIN2
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B);     // Stop BIN1
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, speed);    // Start AIN1
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, 30.0);    // Start BIN2

    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
}