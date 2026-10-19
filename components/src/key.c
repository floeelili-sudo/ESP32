#include "key.h"
#include "esp_err.h"


void Key_Init(void)
{
    gpio_config_t config = {0};

    config.pin_bit_mask = 1ULL << GPIO_NUM_0;
    config.mode = GPIO_MODE_INPUT;
    config.pull_up_en = GPIO_PULLUP_ENABLE;
    config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    config.intr_type = GPIO_INTR_DISABLE;

    ESP_ERROR_CHECK(gpio_config(&config));
}


// 按下并稳定松开后返回1，否则返回0
uint8_t Key_GetStatus(void)
{
    if (BOOT_KEY == 0)
    {
        // 按下消抖
        vTaskDelay(pdMS_TO_TICKS(20));

        if (BOOT_KEY == 0)
        {
            // 等待稳定松开，长按只触发一次
            while (1)
            {
                while (BOOT_KEY == 0)
                {
                    vTaskDelay(pdMS_TO_TICKS(10));
                }

                // 松开消抖
                vTaskDelay(pdMS_TO_TICKS(20));

                if (BOOT_KEY == 1)
                {
                    return 1;
                }
            }
        }
    }

    return 0;
}
