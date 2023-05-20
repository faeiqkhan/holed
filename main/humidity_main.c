#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "dht.h"
#include "ssd1306.h"

#define DHT_GPIO_PIN GPIO_NUM_4
#define OLED_SDA_GPIO_PIN GPIO_NUM_5
#define OLED_SCL_GPIO_PIN GPIO_NUM_6

void sensor_task(void *pvParameters) {
    dht_t dht;
    ssd1306_info_t oled_info;

    // Initialize DHT11 sensor
    dht_init(DHT_TYPE_DHT11, DHT_GPIO_PIN);

    // Initialize OLED display
    ssd1306_128x64_i2c_init(&oled_info, OLED_SDA_GPIO_PIN, OLED_SCL_GPIO_PIN);
    ssd1306_clear_screen(&oled_info);
    ssd1306_refresh(&oled_info, true);

    while (1) {
        // Read humidity and temperature from DHT11
        float humidity, temperature;
        int ret = dht_read_float_data(DHT_TYPE_DHT11, DHT_GPIO_PIN, &humidity, &temperature);
        if (ret == ESP_OK) {
            // Convert float values to strings
            char humidity_str[10];
            char temperature_str[10];
            snprintf(humidity_str, sizeof(humidity_str), "%.2f%%", humidity);
            snprintf(temperature_str, sizeof(temperature_str), "%.2fC", temperature);

            // Clear OLED display
            ssd1306_clear_screen(&oled_info);

            // Print humidity and temperature on OLED
            ssd1306_draw_string(&oled_info, 0, 0, "Humidity:");
            ssd1306_draw_string(&oled_info, 80, 0, humidity_str);
            ssd1306_draw_string(&oled_info, 0, 16, "Temperature:");
            ssd1306_draw_string(&oled_info, 96, 16, temperature_str);

            // Update OLED display
            ssd1306_refresh(&oled_info, true);
        }

        vTaskDelay(pdMS_TO_TICKS(5000));  // Delay between readings
    }
}

void app_main() {
    xTaskCreate(sensor_task, "sensor_task", 4096, NULL, 5, NULL);
}
