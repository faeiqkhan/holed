#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_err.h>
#include <dht/dht.h>
#include <ssd1306/ssd1306.h>
#include <driver/gpio.h>
#include <driver/i2c.h>

#define DHT_GPIO 5 // D1 pin
#define OLED_SDA_GPIO 4 // D2 pin
#define OLED_SCL_GPIO 15 // D8 pin
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_SDA_GPIO OLED_SDA_GPIO
#define I2C_MASTER_SCL_GPIO OLED_SCL_GPIO
#define I2C_MASTER_FREQ_HZ 100000

void temperature_task(void *arg)
{
    ESP_ERROR_CHECK(dht_init(DHT_GPIO, false));

    i2c_config_t i2c_config;
    i2c_config.mode = I2C_MODE_MASTER;
    i2c_config.sda_io_num = I2C_MASTER_SDA_GPIO;
    i2c_config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_config.scl_io_num = I2C_MASTER_SCL_GPIO;
    i2c_config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_config.master.clk_speed = I2C_MASTER_FREQ_HZ;

    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &i2c_config));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, i2c_config.mode, 0, 0, 0));

    ssd1306_init();

    vTaskDelay(2000 / portTICK_PERIOD_MS);

    while (1)
    {
        int humidity = 0;
        int temperature = 0;

        if (dht_read_data(DHT_TYPE_DHT22, DHT_GPIO, &humidity, &temperature) == ESP_OK) {
            char humidity_str[10];
            char temperature_str[10];
            snprintf(humidity_str, sizeof(humidity_str), "%d%%", humidity);
            snprintf(temperature_str, sizeof(temperature_str), "%dC", temperature);

            ssd1306_clearScreen();

            ssd1306_set_font(FONT_FACE_MONOSPACE, 12);
            ssd1306_drawString(0, 0, "Humidity:");
            ssd1306_drawString(80, 0, humidity_str);
            ssd1306_drawString(0, 16, "Temperature:");
            ssd1306_drawString(96, 16, temperature_str);

            ssd1306_display();
        }
        else {
            printf("Fail to get DHT temperature data\n");
        }

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

void app_main() {
    xTaskCreate(temperature_task, "temperature task", 2048, NULL, tskIDLE_PRIORITY, NULL);
}
