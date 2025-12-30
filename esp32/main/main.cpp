#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "lvgl.h"
#include "lv_demos.h"
#include "display_touch.h"
#include "user_encoder_bsp.h"
#include "midi_model.h"
#include "ui_components.h"
#include <memory>

static const char* TAG = "main";

// Global UI state
static PageView* currentPageView = nullptr;

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Starting application");

    // Create display/touch handler on heap to keep it alive
    static DisplayTouch* displayTouch = new DisplayTouch();

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
    ESP_LOGI(TAG, "Creating MIDI UI");

    // Lock the mutex due to the LVGL APIs are not thread-safe
    if (displayTouch->lock(-1))
    {
        // Apply dark theme
        lv_theme_t* theme = lv_theme_default_init(
            lv_display_get_default(),
            lv_palette_main(LV_PALETTE_BLUE),
            lv_palette_main(LV_PALETTE_CYAN),
            true, // dark mode
            LV_FONT_DEFAULT);
        lv_display_set_theme(lv_display_get_default(), theme);

        // Create sample data
        auto page1 = std::make_shared<Page>("Page 1");

        // Add CC parameters
        page1->addParameter(std::make_shared<CCParameter>("DAW", 0, 74));
        page1->addParameter(std::make_shared<CCParameter>("Mic", 0, 71));
        page1->addParameter(std::make_shared<CCParameter>("Guitar", 0, 73));

        // Add Program Change parameter with instrument names
        std::vector<std::string> instruments = {
            "Clean", "Crunch", "Rhythm", "Lead"
        };
        page1->addParameter(std::make_shared<ProgramChangeParameter>(
            "Instrument", 0, instruments)
        );
        // Create UI
        lv_obj_t* screen = lv_screen_active();
        currentPageView = new PageView(screen, page1);

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
            // Bit 0 set - left rotation (decrement)
            ESP_LOGI(TAG, "Encoder: Left rotation detected");

            if (displayTouch->lock(100) && currentPageView)
            {
                currentPageView->handleEncoderRotation(-1);
                displayTouch->unlock();
            }
        }
        if (bits & 0x02)
        {
            // Bit 1 set - right rotation (increment)
            ESP_LOGI(TAG, "Encoder: Right rotation detected");

            if (displayTouch->lock(100) && currentPageView)
            {
                currentPageView->handleEncoderRotation(1);
                displayTouch->unlock();
            }
        }
    }
}
