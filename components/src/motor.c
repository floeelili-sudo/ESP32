#include "motor.h"

void Motor_Init(void)
{
    gpio_config_t pGPIOConfig = {0};

    pGPIOConfig.mode = GPIO_MODE_OUTPUT;
    pGPIOConfig.pin_bit_mask = (1ULL << MOTOR_IA) |
                              (1ULL << MOTOR_IB);
    pGPIOConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    pGPIOConfig.pull_up_en = GPIO_PULLUP_DISABLE;

    gpio_config(&pGPIOConfig);

    Motor_Stop();
}

// 正转：IA高，IB低
void Motor_Run(void)
{
    gpio_set_level(MOTOR_IB, 0);
    gpio_set_level(MOTOR_IA, 1);
}

// 反转：IA低，IB高
void Motor_Back(void)
{
    gpio_set_level(MOTOR_IA, 0);
    gpio_set_level(MOTOR_IB, 1);
}

// 停转：IA高，IB高
void Motor_Stop(void)
{
    gpio_set_level(MOTOR_IA, 1);
    gpio_set_level(MOTOR_IB, 1);
}