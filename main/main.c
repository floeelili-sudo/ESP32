#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "led.h"
#include "key.h"
#include "motor.h"
#include "rgb.h"

ws2812_strip_handle_t ws2812_handle = NULL;

void Run_task(void *ptr);
void Rgb_task(void *ptr);


void app_main(void)
{
    Led_Init();
    Key_Init();
    Motor_Init();

    // 初始化RGB灯
    ws2812_init(WS2812_GPIO_NUM,
                WS2812_LED_NUM,
                &ws2812_handle);

    // // 电机任务
    // xTaskCreatePinnedToCore(
    //     Run_task, "run", 2048, NULL, 5, NULL, 0
    // );

    // RGB任务
    xTaskCreatePinnedToCore(
        Rgb_task, "rgb", 2048, NULL, 5, NULL, 0
    );
}


// // 原来的电机调速任务
// void Run_task(void *ptr)
// {
//     while (1)
//     {
//         Motor_Control(10);
//         vTaskDelay(pdMS_TO_TICKS(1000));

//         Motor_Control(30);
//         vTaskDelay(pdMS_TO_TICKS(1000));

//         Motor_Control(50);
//         vTaskDelay(pdMS_TO_TICKS(1000));

//         Motor_Control(70);
//         vTaskDelay(pdMS_TO_TICKS(1000));

//         Motor_Control(100);
//         vTaskDelay(pdMS_TO_TICKS(1000));
//     }
// }


// RGB循环变色任务
void Rgb_task(void *ptr)
{
    while (1)
    {
        // 依次变成红色
        for (int i = 0; i < WS2812_LED_NUM; i++)
        {
            ws2812_write(ws2812_handle, i, 255, 0, 0);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        // 依次变成绿色
        for (int i = 0; i < WS2812_LED_NUM; i++)
        {
            ws2812_write(ws2812_handle, i, 0, 255, 0);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        // 依次变成蓝色
        for (int i = 0; i < WS2812_LED_NUM; i++)
        {
            ws2812_write(ws2812_handle, i, 0, 0, 255);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}
