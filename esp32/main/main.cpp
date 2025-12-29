#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "lvgl.h"
#include "lv_demos.h"
#include "display_touch.h"

static const char *TAG = "main";

#ifdef Backlight_Testing
#include "lcd_bl_pwm_bsp.h"

void backlight_test_task(void *arg)
{
    for (;;)
    {
        setUpduty(LCD_PWM_MODE_255);
        vTaskDelay(pdMS_TO_TICKS(1000));
        setUpduty(LCD_PWM_MODE_200);
        vTaskDelay(pdMS_TO_TICKS(1000));
        setUpduty(LCD_PWM_MODE_150);
        vTaskDelay(pdMS_TO_TICKS(1000));
        setUpduty(LCD_PWM_MODE_100);
        vTaskDelay(pdMS_TO_TICKS(1000));
        setUpduty(LCD_PWM_MODE_50);
        vTaskDelay(pdMS_TO_TICKS(1000));
        setUpduty(LCD_PWM_MODE_0);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
#endif

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Starting application");

    // Create display/touch handler on heap to keep it alive
    static DisplayTouch *displayTouch = new DisplayTouch();

    esp_err_t ret = displayTouch->init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize display/touch: %s", esp_err_to_name(ret));
        return;
    }

    // Start LVGL task
    ret = displayTouch->startLvglTask();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to start LVGL task: %s", esp_err_to_name(ret));
        return;
    }

#ifdef Backlight_Testing
    xTaskCreate(backlight_test_task, "backlight", 3 * 1024, nullptr, 2, nullptr);
#endif

    // Display LVGL demos
    ESP_LOGI(TAG, "Display LVGL demos");

    // Lock the mutex due to the LVGL APIs are not thread-safe
    if (displayTouch->lock(-1))
    {
        lv_demo_widgets(); // A widgets example
        // lv_demo_music();      // A modern, smartphone-like music player demo
        // lv_demo_stress();     // A stress test for LVGL
        // lv_demo_benchmark();  // A demo to measure the performance of LVGL

        displayTouch->unlock();
    }

    ESP_LOGI(TAG, "Application started successfully");

    // Keep app_main alive - LVGL task is running in background
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
