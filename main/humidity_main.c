#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_err.h>
#include <dht/dht.h>
#include <ssd1306/ssd1306.h>

#define DHT_GPIO 5 // D1 pin
#define OLED_SDA_GPIO 4 // D2 pin
#define OLED_SCL_GPIO 15 // D8 pin

void temperature_task(void *arg)
{
    ESP_ERROR_CHECK(dht_init(DHT_GPIO, false));

    ssd1306_128x64_i2c_init(OLED_SDA_GPIO, OLED_SCL_GPIO);
    ssd1306_display_on();

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

            ssd1306_clear();

            ssd1306_draw_string(0, 0, "Humidity:");
            ssd1306_draw_string(80, 0, humidity_str);
            ssd1306_draw_string(0, 16, "Temperature:");
            ssd1306_draw_string(96, 16, temperature_str);

            ssd1306_refresh();
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
