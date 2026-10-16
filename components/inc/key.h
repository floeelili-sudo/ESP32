#ifndef _KEY_H
#define _KEY_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"


#define BOOT_KEY (gpio_get_level(GPIO_NUM_0))





void Key_Init(void);
uint8_t Key_GetStatus(void);










#endif
