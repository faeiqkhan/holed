#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <ssd1306/ssd1306.h>
#include <driver/i2c.h>
#include <esp_err.h>
#include <dht/dht.h>

#define SCL_PIN 5
#define SDA_PIN 4
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define DHT_GPIO 2 // GPIO pin connected to the DHT22 sensor

void temperature_task(void *arg)
{
    ESP_ERROR_CHECK(dht_init(DHT_GPIO, false));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    // Initialize I2C communication for the OLED display
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = SDA_PIN;
    conf.sda_pullup_en = 1;
    conf.scl_io_num = SCL_PIN;
    conf.scl_pullup_en = 1;
    conf.master.clk_speed = 100000;
    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0));

    // Initialize the SSD1306 OLED display
    ssd1306_t dev = {
        .i2c_port = i2c_master_port,
        .i2c_addr = SSD1306_I2C_ADDR_0,
        .screen = SSD1306_SCREEN, // or SH1106_SCREEN
        .width = DISPLAY_WIDTH,
        .height = DISPLAY_HEIGHT};
    
    ssd1306_init(&dev);

    while (1)
    {
        int humidity = 0;
        int temperature = 0;
        if (dht_read_data(DHT_TYPE_DHT22, DHT_GPIO, &humidity, &temperature) == ESP_OK)
        {
            char display_text[16];
            snprintf(display_text, sizeof(display_text), "Humidity: %d%%", humidity);
            ssd1306_clear_screen(&dev);
            ssd1306_draw_string(&dev, 0, 0, display_text, OLED_COLOR_WHITE);
            snprintf(display_text, sizeof(display_text), "Temperature: %dC", temperature);
            ssd1306_draw_string(&dev, 0, 16, display_text, OLED_COLOR_WHITE);
            ssd1306_init(&dev);
        }
        else
        {
            printf("Fail to get DHT temperature data\n");
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void user_init(void)
{
    xTaskCreate(temperature_task, "temperature task", 256, NULL, 2, NULL);
}
