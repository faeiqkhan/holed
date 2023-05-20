#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "oled/oled.h"
#include "driver/i2c.h"

#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000

#define OLED_I2C_ADDR 0x3C

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

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

    oled_t dev;
    dev.width = OLED_WIDTH;
    dev.height = OLED_HEIGHT;
    dev.color = OLED_COLOR_WHITE;

    uint8_t *fb = (uint8_t *)malloc(OLED_WIDTH * OLED_HEIGHT / 8);
    memset(fb, 0, OLED_WIDTH * OLED_HEIGHT / 8);

    oled_init(&dev);

    oled_draw_string(&dev, fb, &Font_7x10, 0, 0, "Humidity:", OLED_COLOR_WHITE, OLED_COLOR_BLACK);
    oled_draw_string(&dev, fb, &Font_7x10, 0, 16, "Temperature:", OLED_COLOR_WHITE, OLED_COLOR_BLACK);

    char humidity_str[16];
    char temperature_str[16];
    float humidity = 55.5;
    float temperature = 25.5;

    snprintf(humidity_str, sizeof(humidity_str), "%.1f %%", humidity);
    snprintf(temperature_str, sizeof(temperature_str), "%.1f C", temperature);

    oled_draw_string(&dev, fb, &Font_7x10, 64, 0, humidity_str, OLED_COLOR_WHITE, OLED_COLOR_BLACK);
    oled_draw_string(&dev, fb, &Font_7x10, 80, 16, temperature_str, OLED_COLOR_WHITE, OLED_COLOR_BLACK);

    oled_display_frame_buffer(&dev, fb);

    free(fb);
}
