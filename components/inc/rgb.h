#ifndef _RGB_H
#define _RGB_H

#include <stdint.h>
#include "driver/gpio.h"
#include "esp_err.h"

#define WS2812_GPIO_NUM     GPIO_NUM_5
#define WS2812_LED_NUM      2

typedef struct ws2812_strip_t *ws2812_strip_handle_t;

// 初始化
esp_err_t ws2812_init(gpio_num_t gpio, int maxled,
                     ws2812_strip_handle_t *handle);

// 设置指定灯珠的颜色
esp_err_t ws2812_write(ws2812_strip_handle_t handle,
                      uint32_t index,
                      uint32_t r,
                      uint32_t g,
                      uint32_t b);

#endif
