#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ssd1306/ssd1306.h"
#include "driver/i2c.h"

#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000

#define SSD1306_I2C_ADDR 0x3C

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64

void app_main()
{
    i2c_config_t i2c_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 21,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = 22,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ
    };

    i2c_param_config(I2C_MASTER_NUM, &i2c_config);
    i2c_driver_install(I2C_MASTER_NUM, I2C_MODE_MASTER, 0, 0);

    ssd1306_t dev;
    dev.width = SSD1306_WIDTH;
    dev.height = SSD1306_HEIGHT;
    dev.color = SSD1306_COLOR_WHITE;

    uint8_t *fb = (uint8_t *)malloc(SSD1306_WIDTH * SSD1306_HEIGHT / 8);
    memset(fb, 0, SSD1306_WIDTH * SSD1306_HEIGHT / 8);

    ssd1306_init(&dev);

    ssd1306_draw_string(&dev, fb, &Font_7x10, 0, 0, "Humidity:", SSD1306_COLOR_WHITE, SSD1306_COLOR_BLACK);
    ssd1306_draw_string(&dev, fb, &Font_7x10, 0, 16, "Temperature:", SSD1306_COLOR_WHITE, SSD1306_COLOR_BLACK);

    char humidity_str[16];
    char temperature_str[16];
    float humidity = 55.5;
    float temperature = 25.5;

    snprintf(humidity_str, sizeof(humidity_str), "%.1f %%", humidity);
    snprintf(temperature_str, sizeof(temperature_str), "%.1f C", temperature);

    ssd1306_draw_string(&dev, fb, &Font_7x10, 64, 0, humidity_str, SSD1306_COLOR_WHITE, SSD1306_COLOR_BLACK);
    ssd1306_draw_string(&dev, fb, &Font_7x10, 80, 16, temperature_str, SSD1306_COLOR_WHITE, SSD1306_COLOR_BLACK);

    ssd1306_display_frame_buffer(&dev, fb);

    free(fb);
}
