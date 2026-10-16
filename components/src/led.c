#include "led.h"


void Led_Init(void)
{
    gpio_config_t pGPIOConfig={0};
    pGPIOConfig.mode = GPIO_MODE_OUTPUT;
    pGPIOConfig.pin_bit_mask = 1ull << 10;
    pGPIOConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    pGPIOConfig.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&pGPIOConfig);

    gpio_set_level(GPIO_NUM_10, 1);



}