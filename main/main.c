#include <stdio.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"

#include "led.h"
#include "key.h"
#include "rgb.h"
#include "lcd.h"
#include "pic.h"
#include "pic2.h"
#include "pic3.h"

ws2812_strip_handle_t ws2812_handle = NULL;

// 函数声明
static void Show_Picture(uint8_t index);
void Key_task(void *ptr);
void Rgb_task(void *ptr);


// 显示指定图片
static void Show_Picture(uint8_t index)
{
    switch (index)
    {
        case 0:
            Lcd_DisplayPic(0, 0, 320, 240, gImage_pic);
            break;

        case 1:
            Lcd_DisplayPic(0, 0, 320, 240, gImage_pic2);
            break;

        case 2:
            Lcd_DisplayPic(0, 0, 320, 240, gImage_pic3);
            break;

        default:
            break;
    }
}


// 主函数
void app_main(void)
{
    Led_Init();
    Key_Init();

//     spilcd_clear(RED);
//     for (uint8_t i = 0; i < 240; i++)
//     {
//     Lcd_DrawPoint(i,i,BLACK);
// }
    // 初始化LCD
    ESP_ERROR_CHECK(spilcd_init());

    // 初始化RGB灯
    ESP_ERROR_CHECK(
        ws2812_init(
            WS2812_GPIO_NUM,
            WS2812_LED_NUM,
            &ws2812_handle
        )
    );

    // 开机显示第一张图片
    Show_Picture(0);

    // 创建按键切图任务
    BaseType_t ret = xTaskCreatePinnedToCore(
        Key_task,
        "key",
        4096,
        NULL,
        5,
        NULL,
        0
    );

    ESP_ERROR_CHECK(ret == pdPASS ? ESP_OK : ESP_ERR_NO_MEM);

    // 创建RGB循环变色任务
    ret = xTaskCreatePinnedToCore(
        Rgb_task,
        "rgb",
        2048,
        NULL,
        5,
        NULL,
        0
    );

    ESP_ERROR_CHECK(ret == pdPASS ? ESP_OK : ESP_ERR_NO_MEM);
}


// 按键切图任务
void Key_task(void *ptr)
{
    (void)ptr;

    uint8_t pic_index = 0;

    while (1)
    {
        // 按下并松开BOOT按键后切换图片
        if (Key_GetStatus() == 1)
        {
            // 第一张 -> 第二张 -> 第三张 -> 第一张
            pic_index = (pic_index + 1) % 3;
            Show_Picture(pic_index);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}


// RGB循环变色任务
void Rgb_task(void *ptr)
{
    (void)ptr;

    while (1)
    {
        // 依次变成红色
        for (int i = 0; i < WS2812_LED_NUM; i++)
        {
            ESP_ERROR_CHECK(
                ws2812_write(ws2812_handle, i, 255, 0, 0)
            );

            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        // 依次变成绿色
        for (int i = 0; i < WS2812_LED_NUM; i++)
        {
            ESP_ERROR_CHECK(
                ws2812_write(ws2812_handle, i, 0, 255, 0)
            );

            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        // 依次变成蓝色
        for (int i = 0; i < WS2812_LED_NUM; i++)
        {
            ESP_ERROR_CHECK(
                ws2812_write(ws2812_handle, i, 0, 0, 255)
            );

            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}
