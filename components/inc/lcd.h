#ifndef _LCD_H
#define _LCD_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_attr.h"
#include "esp_heap_caps.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/ledc.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_lcd_types.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"

/* 屏幕参数 */
#define BSP_LCD_PIXEL_CLOCK_HZ     (80 * 1000 * 1000)
#define BSP_LCD_SPI_NUM            SPI3_HOST

#define LCD_CMD_BITS              8
#define LCD_PARAM_BITS            8
#define BSP_LCD_BITS_PER_PIXEL    16

#define BSP_LCD_H_RES             320
#define BSP_LCD_V_RES             240

/* 屏幕引脚 */
#define BSP_LCD_SPI_MOSI          GPIO_NUM_40
#define BSP_LCD_SPI_CLK           GPIO_NUM_41
#define BSP_LCD_SPI_CS            GPIO_NUM_8
#define BSP_LCD_DC                GPIO_NUM_39
#define BSP_LCD_RST               GPIO_NUM_NC
#define BSP_LCD_BACKLIGHT         GPIO_NUM_3

/* 背光PWM参数，定时器和通道需与电机分开 */
#define LEDC0_TIMER_BIT           LEDC_TIMER_13_BIT
#define BSP_LCD_BL_TIMER          LEDC_TIMER_1
#define BSP_LCD_BL_CHANNEL        LEDC_CHANNEL_2

/* 横屏方向 */
#define DISPLAY_MIRROR_X          false
#define DISPLAY_MIRROR_Y          true
#define DISPLAY_SWAP_XY           true

/* 常用颜色，RGB565格式 */
#define WHITE                    0xFFFF
#define BLACK                    0x0000
#define RED                      0xF800
#define GREEN                    0x07E0
#define BLUE                     0x001F
#define MAGENTA                  0xF81F
#define YELLOW                   0xFFE0
#define CYAN                     0x07FF

/* 其他颜色 */
#define BROWN                    0xBC40
#define BRRED                    0xFC07
#define GRAY                     0x8430
#define DARKBLUE                 0x01CF
#define LIGHTBLUE                0x7D7C
#define GRAYBLUE                 0x5458
#define LIGHTGREEN               0x841F
#define LGRAY                    0xC618
#define LGRAYBLUE                0xA651
#define LBBLUE                   0x2B12

/* 屏幕信息 */
typedef struct
{
    uint32_t pwidth;
    uint32_t pheight;
    uint8_t dir;
    uint16_t width;
    uint16_t height;
} _spilcd_dev;

extern esp_lcd_panel_handle_t panel_handle;
extern _spilcd_dev spilcddev;
extern volatile uint8_t refresh_done_flag;

/* PWM定时器初始化 */
void ledc_timer_init(ledc_timer_bit_t resolution,
                     uint32_t freq_hz,
                     ledc_mode_t speed_mode,
                     ledc_timer_t timer_num);

/* 背光初始化 */
void spilcd_back_light_init(gpio_num_t pin,
                            ledc_timer_t timer_num,
                            ledc_timer_bit_t ledc_timer_bit,
                            ledc_channel_t channel,
                            bool is_output_invert);

/* 设置背光亮度，范围0～100 */
esp_err_t spilcd_set_brightness(uint8_t brightness);

/* 初始化屏幕驱动 */
esp_err_t bsp_display_new(void);

/* 初始化屏幕和背光 */
esp_err_t spilcd_init(void);

/* 全屏填充颜色 */
esp_err_t spilcd_clear(uint16_t color);

/* 设置方向，0为竖屏，1为横屏 */
void spilcd_display_dir(uint8_t dir);

/* 设置整屏颜色 */
void lcd_set_color(uint16_t color);

void Lcd_DisplayPic(uint16_t x, uint16_t y,
                    uint16_t w, uint16_t h,
                    const uint8_t *pic);


void Lcd_DrawPoint(uint16_t x,uint16_t y,uint16_t color);

#endif