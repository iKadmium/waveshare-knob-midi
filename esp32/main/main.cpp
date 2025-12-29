#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "lvgl.h"
#include "lv_demos.h"
#include "display_touch.h"
#include "user_encoder_bsp.h"

static const char *TAG = "main";

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

    // Initialize rotary encoder
    ret = displayTouch->initEncoder();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize encoder: %s", esp_err_to_name(ret));
        return;
    }

    // Start LVGL task
    ret = displayTouch->startLvglTask();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to start LVGL task: %s", esp_err_to_name(ret));
        return;
    }

    // Display LVGL demos
    ESP_LOGI(TAG, "Display LVGL demos");

    // Lock the mutex due to the LVGL APIs are not thread-safe
    if (displayTouch->lock(-1))
    {
        lv_demo_widgets(); // A widgets example

        displayTouch->unlock();
    }

    ESP_LOGI(TAG, "Application started successfully");

    // Main loop - monitor encoder events
    while (1)
    {
        // Wait for encoder events (left or right rotation)
        EventBits_t bits = xEventGroupWaitBits(
            knob_even_,
            0x03,    // Wait for bit 0 (left) or bit 1 (right)
            pdTRUE,  // Clear bits on exit
            pdFALSE, // Wait for ANY bit (not all)
            pdMS_TO_TICKS(1000));

        if (bits & 0x01)
        {
            // Bit 0 set - left rotation
            ESP_LOGI(TAG, "Encoder: Left rotation detected");
        }
        if (bits & 0x02)
        {
            // Bit 1 set - right rotation
            ESP_LOGI(TAG, "Encoder: Right rotation detected");
        }
    }
}
