#include "lcd.h"

#define TAG "spilcd"

ledc_channel_config_t lcd_channel_ = {0};
ledc_timer_bit_t LEDC_TIMER_BIT = LEDC_TIMER_13_BIT;

DRAM_ATTR volatile uint8_t refresh_done_flag = 0;

esp_lcd_panel_handle_t panel_handle = NULL;
_spilcd_dev spilcddev = {0};


/* PWM定时器初始化 */
void ledc_timer_init(ledc_timer_bit_t resolution,
                     uint32_t freq_hz,
                     ledc_mode_t speed_mode,
                     ledc_timer_t timer_num)
{
    ledc_timer_config_t ledc_timer = {0};

    ledc_timer.duty_resolution = resolution;
    ledc_timer.freq_hz = freq_hz;
    ledc_timer.speed_mode = speed_mode;
    ledc_timer.timer_num = timer_num;
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;

    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
}


/* 背光初始化 */
void spilcd_back_light_init(gpio_num_t pin,
                            ledc_timer_t timer_num,
                            ledc_timer_bit_t ledc_timer_bit,
                            ledc_channel_t channel,
                            bool is_output_invert)
{
    LEDC_TIMER_BIT = ledc_timer_bit;

    lcd_channel_.channel = channel;
    lcd_channel_.duty = 0;
    lcd_channel_.gpio_num = pin;
    lcd_channel_.speed_mode = LEDC_LOW_SPEED_MODE;
    lcd_channel_.hpoint = 0;
    lcd_channel_.timer_sel = timer_num;
    lcd_channel_.flags.output_invert = is_output_invert;

    ESP_ERROR_CHECK(ledc_channel_config(&lcd_channel_));
}


/* 设置背光亮度 */
esp_err_t spilcd_set_brightness(uint8_t brightness)
{
    if (brightness > 100)
    {
        brightness = 100;
    }

    uint32_t duty =
        ((1UL << LEDC_TIMER_BIT) - 1) * brightness / 100;

    esp_err_t ret = ledc_set_duty(
        LEDC_LOW_SPEED_MODE,
        lcd_channel_.channel,
        duty
    );

    if (ret != ESP_OK)
    {
        return ret;
    }

    return ledc_update_duty(
        LEDC_LOW_SPEED_MODE,
        lcd_channel_.channel
    );
}


/* 屏幕数据发送完成回调 */
static bool IRAM_ATTR notify_lcd_flush_ready(
    esp_lcd_panel_io_handle_t panel_io,
    esp_lcd_panel_io_event_data_t *edata,
    void *user_ctx)
{
    (void)panel_io;
    (void)edata;
    (void)user_ctx;

    refresh_done_flag = 1;

    return false;
}


/* 设置屏幕方向 */
void spilcd_display_dir(uint8_t dir)
{
    if (panel_handle == NULL)
    {
        ESP_LOGE(TAG, "LCD is not initialized");
        return;
    }

    if (dir > 1)
    {
        ESP_LOGE(TAG, "Invalid display direction");
        return;
    }

    if (dir == 0)
    {
        /* 竖屏 */
        ESP_ERROR_CHECK(
            esp_lcd_panel_swap_xy(
                panel_handle,
                !DISPLAY_SWAP_XY
            )
        );

        ESP_ERROR_CHECK(
            esp_lcd_panel_mirror(
                panel_handle,
                DISPLAY_MIRROR_Y,
                !DISPLAY_MIRROR_X
            )
        );

        spilcddev.width = spilcddev.pheight;
        spilcddev.height = spilcddev.pwidth;
    }
    else
    {
        /* 横屏 */
        ESP_ERROR_CHECK(
            esp_lcd_panel_swap_xy(
                panel_handle,
                DISPLAY_SWAP_XY
            )
        );

        ESP_ERROR_CHECK(
            esp_lcd_panel_mirror(
                panel_handle,
                DISPLAY_MIRROR_X,
                DISPLAY_MIRROR_Y
            )
        );

        spilcddev.width = spilcddev.pwidth;
        spilcddev.height = spilcddev.pheight;
    }

    spilcddev.dir = dir;
}


/* 全屏填充颜色 */
esp_err_t spilcd_clear(uint16_t color)
{
    if (panel_handle == NULL ||
        spilcddev.width == 0 ||
        spilcddev.height == 0)
    {
        return ESP_ERR_INVALID_STATE;
    }

    /* 每次发送40行 */
    const uint16_t lines = 40;

    size_t count = (size_t)spilcddev.width * lines;

    uint16_t *buffer = heap_caps_malloc(
        count * sizeof(uint16_t),
        MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL
    );

    if (buffer == NULL)
    {
        ESP_LOGE(TAG, "Memory for bitmap is not enough");
        return ESP_ERR_NO_MEM;
    }

    /* RGB565高字节先发送 */
    uint16_t color_tmp =
        (uint16_t)((color << 8) | (color >> 8));

    for (size_t i = 0; i < count; i++)
    {
        buffer[i] = color_tmp;
    }

    esp_err_t ret = ESP_OK;

    for (uint16_t y = 0; y < spilcddev.height; y += lines)
    {
        uint16_t end_y = y + lines;

        /* 最后一块不能超过屏幕高度 */
        if (end_y > spilcddev.height)
        {
            end_y = spilcddev.height;
        }

        /* 必须在提交数据之前清零 */
        refresh_done_flag = 0;

        ret = esp_lcd_panel_draw_bitmap(
            panel_handle,
            0,
            y,
            spilcddev.width,
            end_y,
            buffer
        );

        if (ret != ESP_OK)
        {
            ESP_LOGE(
                TAG,
                "Draw bitmap failed: %s",
                esp_err_to_name(ret)
            );
            break;
        }

        /* 等待这一块发送完成，再继续使用缓存 */
        while (refresh_done_flag == 0)
        {
            vTaskDelay(1);
        }
    }

    heap_caps_free(buffer);

    return ret;
}


/* 设置整屏颜色 */
void lcd_set_color(uint16_t color)
{
    esp_err_t ret = spilcd_clear(color);

    if (ret != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Set color failed: %s",
            esp_err_to_name(ret)
        );
    }
}


/* 液晶屏驱动初始化 */
esp_err_t bsp_display_new(void)
{
    if (panel_handle != NULL)
    {
        return ESP_OK;
    }

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_err_t ret = ESP_OK;

    /* SPI总线配置 */
    const spi_bus_config_t buscfg = {
        .sclk_io_num = BSP_LCD_SPI_CLK,
        .mosi_io_num = BSP_LCD_SPI_MOSI,
        .miso_io_num = GPIO_NUM_NC,
        .quadwp_io_num = GPIO_NUM_NC,
        .quadhd_io_num = GPIO_NUM_NC,
        .max_transfer_sz =
            BSP_LCD_H_RES * BSP_LCD_V_RES * sizeof(uint16_t),
    };

    ret = spi_bus_initialize(
        BSP_LCD_SPI_NUM,
        &buscfg,
        SPI_DMA_CH_AUTO
    );

    if (ret != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "SPI init failed: %s",
            esp_err_to_name(ret)
        );
        return ret;
    }

    /* 屏幕SPI接口配置 */
    const esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = BSP_LCD_DC,
        .cs_gpio_num = BSP_LCD_SPI_CS,
        .pclk_hz = BSP_LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = LCD_CMD_BITS,
        .lcd_param_bits = LCD_PARAM_BITS,
        .spi_mode = 2,
        .trans_queue_depth = 10,
    };

    ESP_GOTO_ON_ERROR(
        esp_lcd_new_panel_io_spi(
            (esp_lcd_spi_bus_handle_t)BSP_LCD_SPI_NUM,
            &io_config,
            &io_handle
        ),
        err,
        TAG,
        "New panel IO failed"
    );

    /* ST7789驱动配置 */
    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = BSP_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = BSP_LCD_BITS_PER_PIXEL,
    };

    ESP_GOTO_ON_ERROR(
        esp_lcd_new_panel_st7789(
            io_handle,
            &panel_config,
            &panel_handle
        ),
        err,
        TAG,
        "New panel failed"
    );

    /* 注册刷新完成回调 */
    const esp_lcd_panel_io_callbacks_t cbs = {
        .on_color_trans_done = notify_lcd_flush_ready,
    };

    ESP_GOTO_ON_ERROR(
        esp_lcd_panel_io_register_event_callbacks(
            io_handle,
            &cbs,
            NULL
        ),
        err,
        TAG,
        "Register callback failed"
    );

    /* 屏幕复位 */
    ESP_GOTO_ON_ERROR(
        esp_lcd_panel_reset(panel_handle),
        err,
        TAG,
        "Panel reset failed"
    );

    /* 初始化屏幕寄存器 */
    ESP_GOTO_ON_ERROR(
        esp_lcd_panel_init(panel_handle),
        err,
        TAG,
        "Panel init failed"
    );

    /* 设置颜色反转 */
    ESP_GOTO_ON_ERROR(
        esp_lcd_panel_invert_color(panel_handle, true),
        err,
        TAG,
        "Invert color failed"
    );

    spilcddev.pwidth = BSP_LCD_H_RES;
    spilcddev.pheight = BSP_LCD_V_RES;

    /* 横屏 */
    spilcd_display_dir(1);

    /* 按文档目标，初始化后全屏显示白色 */
    ESP_GOTO_ON_ERROR(
        spilcd_clear(WHITE),
        err,
        TAG,
        "Clear screen failed"
    );

    return ESP_OK;

err:
    if (panel_handle != NULL)
    {
        esp_lcd_panel_del(panel_handle);
        panel_handle = NULL;
    }

    if (io_handle != NULL)
    {
        esp_lcd_panel_io_del(io_handle);
    }

    spi_bus_free(BSP_LCD_SPI_NUM);

    memset(&spilcddev, 0, sizeof(spilcddev));
    refresh_done_flag = 0;

    return ret;
}


/* 屏幕和背光初始化 */
esp_err_t spilcd_init(void)
{
    /* 背光使用独立的PWM定时器和通道 */
    ledc_timer_init(
        LEDC0_TIMER_BIT,
        4000,
        LEDC_LOW_SPEED_MODE,
        BSP_LCD_BL_TIMER
    );

    spilcd_back_light_init(
        BSP_LCD_BACKLIGHT,
        BSP_LCD_BL_TIMER,
        LEDC0_TIMER_BIT,
        BSP_LCD_BL_CHANNEL,
        false
    );

    esp_err_t ret = bsp_display_new();

    if (ret != ESP_OK)
    {
        return ret;
    }

    /* 打开屏幕显示 */
    ret = esp_lcd_panel_disp_on_off(panel_handle, true);

    if (ret != ESP_OK)
    {
        return ret;
    }

    /* 打开背光 */
    return spilcd_set_brightness(100);
}

// 显示图片，并等待传输完成
void Lcd_DisplayPic(uint16_t x, uint16_t y,
                    uint16_t w, uint16_t h,
                    const uint8_t *pic)
{
    refresh_done_flag = 0;

    ESP_ERROR_CHECK(
        esp_lcd_panel_draw_bitmap(
            panel_handle,
            x,
            y,
            x + w,
            y + h,
            pic
        )
    );

    // 等待图片发送完成
    while (refresh_done_flag == 0)
    {
        vTaskDelay(1);
    }
}

void Lcd_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
    uint16_t color_temp = (color & 0x00ff) << 8 | (color & 0xff00) >> 8;
    esp_lcd_panel_draw_bitmap(panel_handle,x,y,x+1,y+1,&color_temp);

}