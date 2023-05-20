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
#define I2C_MASTER_FREQ_HZ 100000

void temperature_task(void *arg)
{
    ESP_ERROR_CHECK(dht_init(DHT_GPIO, false));

    i2c_master_init();

    ssd1306_t dev;
    dev.port = I2C_MASTER_NUM;
    dev.addr = SSD1306_I2C_ADDR_DEFAULT;
    dev.width = SSD1306_WIDTH_128;
    dev.height = SSD1306_HEIGHT_64;

    ssd1306_init(&dev);

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

            ssd1306_clear_screen(&dev);

            ssd1306_set_text_size(&dev, 1);
            ssd1306_draw_string(&dev, 0, 0, "Humidity:");
            ssd1306_draw_string(&dev, 80, 0, humidity_str);
            ssd1306_draw_string(&dev, 0, 16, "Temperature:");
            ssd1306_draw_string(&dev, 96, 16, temperature_str);

            ssd1306_refresh(&dev, true);
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
