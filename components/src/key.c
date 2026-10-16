#include "key.h"

void Key_Init(void)
{
    gpio_config_t pGPIOConfig={0};
    pGPIOConfig.mode = GPIO_MODE_INPUT;
    pGPIOConfig.pin_bit_mask = 1ull << GPIO_NUM_0;
    pGPIOConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    pGPIOConfig.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&pGPIOConfig);
    
}

uint8_t Key_GetStatus(void)
{
    if(!BOOT_KEY)
    {
        vTaskDelay(30 / portTICK_PERIOD_MS);
        while(!BOOT_KEY);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        return 1;

    }
    return 0;


}
