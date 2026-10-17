#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "led.h"
#include "key.h"
#include "motor.h"  

void Run_task(void *ptr);


void app_main(void)
{
    Led_Init();
    Key_Init();
    Motor_Init();  


     xTaskCreatePinnedToCore(
        Run_task, "run", 2048, NULL, 5, NULL, 0
    );
}


// void Run_task(void *ptr)
// {
//     printf("Start Run Task\r\n");

//     while (1)
//     {
//         printf("Task Run\r\n");
//         vTaskDelay((500));
//     }

// }


void Run_task(void *ptr)
{
    while (1)
    {
        Motor_Control(10);
        vTaskDelay(pdMS_TO_TICKS(1000));

        Motor_Control(30);
        vTaskDelay(pdMS_TO_TICKS(1000));

        Motor_Control(50);
        vTaskDelay(pdMS_TO_TICKS(1000));

        Motor_Control(70);
        vTaskDelay(pdMS_TO_TICKS(1000));

        Motor_Control(100);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// void Run_task(void *ptr)
// {
//     uint8_t led_state = 0;

//     while(1)
//     {
//         if(Key_GetStatus())
//         {
//             printf("按下按键,LED状态翻转\r\n");

//             led_state = !led_state; // 状态翻转（0变1，1变0）

//             if(led_state)
//             {
//                 gpio_set_level(GPIO_NUM_10, 0); // 打开LED
//             }
//             else
//             {
//                 gpio_set_level(GPIO_NUM_10, 1); // 关闭LED
//             }
//         }
//         vTaskDelay(10 / portTICK_PERIOD_MS);
//     }
// }

// // 电机循环
// void Motor_task(void *ptr)
// {
//     while(1)
//     {
//         printf("电机正转\r\n");
//         Motor_Run();
//         vTaskDelay(pdMS_TO_TICKS(500));

//         printf("电机反转\r\n");
//         Motor_Back();
//         vTaskDelay(pdMS_TO_TICKS(500));

//         printf("电机停转\r\n");
//         Motor_Stop();
//         vTaskDelay(pdMS_TO_TICKS(500));
//     }
// }
