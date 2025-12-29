#ifndef DISPLAY_TOUCH_H
#define DISPLAY_TOUCH_H

#include <atomic>

#ifdef __cplusplus
extern "C"
{
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_timer.h"
#include "lvgl.h"
#include "user_config.h"

#ifdef __cplusplus
}
#endif

/**
 * @brief Display and Touch management class
 */
class DisplayTouch
{
public:
    /**
     * @brief Constructor
     */
    DisplayTouch();

    /**
     * @brief Destructor
     */
    ~DisplayTouch();

    /**
     * @brief Initialize the display and touch hardware
     * @return ESP_OK on success
     */
    esp_err_t init();

    /**
     * @brief Initialize the rotary encoder
     * @return ESP_OK on success
     */
    esp_err_t initEncoder();

    /**
     * @brief Start the LVGL task
     * @return ESP_OK on success
     */
    esp_err_t startLvglTask();

    /**
     * @brief Lock LVGL mutex
     * @param timeout_ms Timeout in milliseconds (-1 for infinite)
     * @return true if locked successfully
     */
    bool lock(int timeout_ms);

    /**
     * @brief Unlock LVGL mutex
     */
    void unlock();

    /**
     * @brief Get the LVGL display object
     * @return Pointer to the LVGL display
     */
    lv_display_t *getDisplay() { return disp; }

private:
    // Hardware handles
    esp_lcd_panel_io_handle_t io_handle;
    esp_lcd_panel_handle_t panel_handle;
    esp_timer_handle_t lvgl_tick_timer;
    SemaphoreHandle_t lvgl_mux;
    lv_display_t *disp;

    // Note: Encoder is now handled by user_encoder_bsp component

    // LVGL callbacks
    static void lvglFlushCb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
    static void lvglRounderCb(lv_event_t *e);
    static bool notifyLvglFlushReady(esp_lcd_panel_io_handle_t panel_io,
                                     esp_lcd_panel_io_event_data_t *edata,
                                     void *user_ctx);
    static void increaseLvglTick(void *arg);

#if EXAMPLE_USE_TOUCH
    static void lvglTouchCb(lv_indev_t *indev, lv_indev_data_t *data);
#endif

    // Task functions
    static void lvglPortTask(void *arg);
};

#endif // DISPLAY_TOUCH_H
