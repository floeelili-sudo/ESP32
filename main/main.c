#include <stdio.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"

#include "lcd.h"

// 暂时不使用的模块
// #include "led.h"
// #include "key.h"
// #include "motor.h"
// #include "rgb.h"
// #include "pic.h"
// #include "pic2.h"
// #include "pic3.h"

// ws2812_strip_handle_t ws2812_handle = NULL;


// 8×16字母ABC字模
// 逐行存储，每行1字节，高位对应左侧像素
static uint8_t engABC[3][16] = {
    // A
    {
        0x00,0x00,0xC0,0x38,0xE0,0x00,0x00,0x00,
        0x20,0x3C,0x23,0x02,0x02,0x27,0x38,0x20
    },

    // B
    {
        0x08,0xF8,0x88,0x88,0x88,0x70,0x00,0x00,
        0x20,0x3F,0x20,0x20,0x20,0x11,0x0E,0x00
    },

    // C
    {
        0xC0,0x30,0x08,0x08,0x08,0x08,0x38,0x00,
        0x07,0x18,0x20,0x20,0x20,0x10,0x08,0x00
    }
};

// 暂时不使用的函数声明
// static void Show_Picture(uint8_t index);
// void Key_task(void *ptr);
// void Rgb_task(void *ptr);


// 主函数
void app_main(void)
{
    // Led_Init();
    // Key_Init();
    // Motor_Init();

    // 初始化LCD和背光
    ESP_ERROR_CHECK(spilcd_init());

    // 暂时不初始化RGB灯
    /*
    ESP_ERROR_CHECK(
        ws2812_init(
            WS2812_GPIO_NUM,
            WS2812_LED_NUM,
            &ws2812_handle
        )
    );
    */

    // 全屏填充红色
    ESP_ERROR_CHECK(spilcd_clear(RED));

    // 绘制黑色对角线
    for (uint16_t i = 0; i < 240; i++)
    {
        Lcd_DrawPoint(i, i, BLACK);
    }

    // 显示字母A：黑色文字、黄色背景
    // 显示ABC，每个字符宽8像素
    Lcd_DisplayChar(0,  0, BLACK, YELLOW, engABC[0]);
    Lcd_DisplayChar(8,  0, BLACK, YELLOW, engABC[1]);
    Lcd_DisplayChar(16, 0, BLACK, YELLOW, engABC[2]);

    // 保持当前画面，不再显示图片
    // vTaskDelay(pdMS_TO_TICKS(2000));
    // Show_Picture(0);

    // 暂时不创建按键切图任务
    /*
    BaseType_t ret = xTaskCreatePinnedToCore(
        Key_task,
        "key",
        4096,
        NULL,
        5,
        NULL,
        0
    );

    ESP_ERROR_CHECK(
        ret == pdPASS ? ESP_OK : ESP_ERR_NO_MEM
    );
    */

    // 暂时不创建RGB任务
    /*
    BaseType_t rgb_ret = xTaskCreatePinnedToCore(
        Rgb_task,
        "rgb",
        2048,
        NULL,
        5,
        NULL,
        0
    );

    ESP_ERROR_CHECK(
        rgb_ret == pdPASS ? ESP_OK : ESP_ERR_NO_MEM
    );
    */
}


// 图片显示函数，暂时注释
/*
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
*/


// 按键切图任务，暂时注释
/*
void Key_task(void *ptr)
{
    (void)ptr;

    uint8_t pic_index = 0;

    while (1)
    {
        if (Key_GetStatus() == 1)
        {
            pic_index = (pic_index + 1) % 3;
            Show_Picture(pic_index);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
*/


// RGB循环变色任务，暂时注释
/*
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
*/
