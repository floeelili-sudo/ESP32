#include "motor.h"


void Motor_Init(void)
{
   
    ledc_timer_config_t timer_conf = {0};
    timer_conf.duty_resolution = LEDC_TIMER_13_BIT;
    timer_conf.freq_hz = 1000;
    timer_conf.speed_mode = LEDC_LOW_SPEED_MODE;
    timer_conf.timer_num = LEDC_TIMER_1;
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t ledc_confIA={0};
    ledc_confIA.channel = LEDC_CHANNEL_2;
    ledc_confIA.duty = 0;
    ledc_confIA.gpio_num = 46;
    ledc_confIA.hpoint =0 ;
    ledc_confIA.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_confIA.timer_sel = LEDC_TIMER_1;
    ledc_channel_config(&ledc_confIA);

    ledc_channel_config_t ledc_confIB={0};
    ledc_confIB.channel = LEDC_CHANNEL_3;
    ledc_confIB.duty = 0;
    ledc_confIB.gpio_num = 9;
    ledc_confIB.hpoint =0 ;
    ledc_confIB.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_confIB.timer_sel = LEDC_TIMER_1;
    ledc_channel_config(&ledc_confIB);
}

void Motor_IA_PWM(uint16_t speed)
{
    float pwm_value = 0;
    pwm_value = 8192 * speed  / 100.0f;

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, (int)pwm_value);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);


}

void Motor_IB_PWM(uint16_t speed)
{
     float pwm_value = 0;
    pwm_value = 8192 * speed  / 100.0f;

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3, (int)pwm_value);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3);


}

void Motor_Control(int speed)
{
    if(speed >0)
    {
        Motor_IA_PWM(speed);
        Motor_IB_PWM(0);
    }

    else if(speed  < 0)
    {
        Motor_IA_PWM(0);
        Motor_IB_PWM(-speed);
    }


     else if(speed  == 0)
    {
        Motor_IA_PWM(0);
        Motor_IB_PWM(0);
    }


}

