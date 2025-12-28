#include <stdio.h>
#include "hal.h"

#ifdef ESP_PLATFORM
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_lcd_sh8601.h"
#include "user_config.h"
#else
#include <pthread.h>
#include <unistd.h>
#include "sdl/sdl.h"
// Define constants for native build
#define EXAMPLE_LCD_H_RES 360
#define EXAMPLE_LCD_V_RES 360
#define EXAMPLE_LVGL_BUF_HEIGHT (EXAMPLE_LCD_V_RES / 10)
#define EXAMPLE_LVGL_TICK_PERIOD_MS 2
#define EXAMPLE_LVGL_TASK_MAX_DELAY_MS 500
#define EXAMPLE_LVGL_TASK_MIN_DELAY_MS 5
#define EXAMPLE_LVGL_TASK_STACK_SIZE (4 * 1024)
#define EXAMPLE_LVGL_TASK_PRIORITY 2
#define EXAMPLE_USE_TOUCH 1
#endif

#include "demos/lv_demos.h"

static const char *TAG = "main";

#ifdef ESP_PLATFORM
static SemaphoreHandle_t lvgl_mux = NULL;
#else
static pthread_mutex_t lvgl_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

#if CONFIG_LV_COLOR_DEPTH == 32
#define LCD_BIT_PER_PIXEL (24)
#elif CONFIG_LV_COLOR_DEPTH == 16
#define LCD_BIT_PER_PIXEL (16)
#endif

// LVGL callbacks
#ifdef ESP_PLATFORM
static bool example_notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io,
                                            esp_lcd_panel_io_event_data_t *edata,
                                            void *user_ctx)
{
    lv_disp_drv_t *disp_driver = (lv_disp_drv_t *)user_ctx;
    lv_disp_flush_ready(disp_driver);
    return false;
}
#endif

static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    const int offsetx1 = area->x1;
    const int offsetx2 = area->x2;
    const int offsety1 = area->y1;
    const int offsety2 = area->y2;

    // Debug: Check the buffer pointer and first few pixels
    static int flush_debug_count = 0;
    if (flush_debug_count < 3 || (flush_debug_count >= 10 && flush_debug_count < 13))
    {
        printf("[main/flush] Flush %d: buffer=%p, area=(%d,%d)-(%d,%d)\n",
               flush_debug_count, (void *)color_map, offsetx1, offsety1, offsetx2, offsety2);
        if (color_map)
        {
            uint16_t *pixels = (uint16_t *)color_map;
            printf("[main/flush] First 8 pixels: 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x\n",
                   pixels[0], pixels[1], pixels[2], pixels[3], pixels[4], pixels[5], pixels[6], pixels[7]);
        }
    }
    flush_debug_count++;

#if LCD_BIT_PER_PIXEL == 24
    uint8_t *to = (uint8_t *)color_map;
    uint8_t temp = 0;
    uint16_t pixel_num = (offsetx2 - offsetx1 + 1) * (offsety2 - offsety1 + 1);

    // Special dealing for first pixel
    temp = color_map[0].ch.blue;
    *to++ = color_map[0].ch.red;
    *to++ = color_map[0].ch.green;
    *to++ = temp;
    // Normal dealing for other pixels
    for (int i = 1; i < pixel_num; i++)
    {
        *to++ = color_map[i].ch.red;
        *to++ = color_map[i].ch.green;
        *to++ = color_map[i].ch.blue;
    }
#endif

    // Use HAL to flush display
    size_t pixel_count = (offsetx2 - offsetx1 + 1) * (offsety2 - offsety1 + 1);
    g_hal->display.flush_display(offsetx1, offsety1, offsetx2, offsety2,
                                 color_map, pixel_count * sizeof(lv_color_t));

    lv_disp_flush_ready(drv);
}

void lvgl_rounder_cb(struct _lv_disp_drv_t *disp_drv, lv_area_t *area)
{
    uint16_t x1 = area->x1;
    uint16_t x2 = area->x2;
    uint16_t y1 = area->y1;
    uint16_t y2 = area->y2;

    // round the start of coordinate down to the nearest 2M number
    area->x1 = (x1 >> 1) << 1;
    area->y1 = (y1 >> 1) << 1;
    // round the end of coordinate up to the nearest 2N+1 number
    area->x2 = ((x2 >> 1) << 1) + 1;
    area->y2 = ((y2 >> 1) << 1) + 1;
}

#if EXAMPLE_USE_TOUCH || !defined(ESP_PLATFORM)
static void lvgl_touch_cb(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    uint16_t tp_x = 0;
    uint16_t tp_y = 0;

    bool pressed = g_hal->touch.read_touch(&tp_x, &tp_y);

    if (pressed)
    {
#ifdef EXAMPLE_Rotate_90
        data->point.x = tp_y;
        data->point.y = (EXAMPLE_LCD_V_RES - tp_x);
#else
        data->point.x = tp_x;
        data->point.y = tp_y;
#endif
        if (data->point.x > EXAMPLE_LCD_H_RES)
            data->point.x = EXAMPLE_LCD_H_RES;
        if (data->point.y > EXAMPLE_LCD_V_RES)
            data->point.y = EXAMPLE_LCD_V_RES;
        data->state = LV_INDEV_STATE_PRESSED;
    }
    else
    {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}
#endif

static void increase_lvgl_tick(void *arg)
{
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

#ifdef ESP_PLATFORM
static bool lvgl_lock(int timeout_ms)
{
    assert(lvgl_mux && "lvgl_mux must be initialized");
    const TickType_t timeout_ticks = (timeout_ms == -1) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return xSemaphoreTake(lvgl_mux, timeout_ticks) == pdTRUE;
}

static void lvgl_unlock(void)
{
    assert(lvgl_mux && "lvgl_mux must be initialized");
    xSemaphoreGive(lvgl_mux);
}

static void lvgl_port_task(void *arg)
{
    g_hal->system.log(TAG, "Starting LVGL task");
    uint32_t task_delay_ms = EXAMPLE_LVGL_TASK_MAX_DELAY_MS;
    while (1)
    {
        if (lvgl_lock(-1))
        {
            task_delay_ms = lv_timer_handler();
            lvgl_unlock();
        }
        if (task_delay_ms > EXAMPLE_LVGL_TASK_MAX_DELAY_MS)
        {
            task_delay_ms = EXAMPLE_LVGL_TASK_MAX_DELAY_MS;
        }
        else if (task_delay_ms < EXAMPLE_LVGL_TASK_MIN_DELAY_MS)
        {
            task_delay_ms = EXAMPLE_LVGL_TASK_MIN_DELAY_MS;
        }
        vTaskDelay(pdMS_TO_TICKS(task_delay_ms));
    }
}
#else
static bool lvgl_lock(int timeout_ms)
{
    (void)timeout_ms;
    pthread_mutex_lock(&lvgl_mutex);
    return true;
}

static void lvgl_unlock(void)
{
    pthread_mutex_unlock(&lvgl_mutex);
}

static void *lvgl_port_task(void *arg)
{
    (void)arg;
    g_hal->system.log(TAG, "Starting LVGL task");
    int loop_count = 0;
    while (1)
    {
        if (lvgl_lock(-1))
        {
            uint32_t task_delay_ms = lv_timer_handler();
            lvgl_unlock();

            if (loop_count < 5)
            {
                printf("[%s] LVGL handler loop #%d, delay: %u ms\n", TAG, loop_count++, task_delay_ms);
            }

            if (task_delay_ms > EXAMPLE_LVGL_TASK_MAX_DELAY_MS)
            {
                task_delay_ms = EXAMPLE_LVGL_TASK_MAX_DELAY_MS;
            }
            else if (task_delay_ms < EXAMPLE_LVGL_TASK_MIN_DELAY_MS)
            {
                task_delay_ms = EXAMPLE_LVGL_TASK_MIN_DELAY_MS;
            }
            g_hal->system.delay_ms(task_delay_ms);
        }
    }
    return NULL;
}
#endif

#ifdef ESP_PLATFORM
extern "C" void app_main(void)
#else
int main(int argc, char **argv)
#endif
{
    // Initialize HAL
    if (hal_init() != 0)
    {
        fprintf(stderr, "Failed to initialize HAL\n");
#ifndef ESP_PLATFORM
        return -1;
#else
        return;
#endif
    }

    // Configure HAL
    hal_config_t hal_config = {
        .screen_width = EXAMPLE_LCD_H_RES,
        .screen_height = EXAMPLE_LCD_V_RES,
        .buffer_height = EXAMPLE_LVGL_BUF_HEIGHT,
#if EXAMPLE_USE_TOUCH
        .enable_touch = true,
#else
        .enable_touch = false,
#endif
    };

    // Initialize display
    if (g_hal->display.init(&hal_config) != 0)
    {
        fprintf(stderr, "Failed to initialize display\n");
#ifndef ESP_PLATFORM
        return -1;
#else
        return;
#endif
    }

    // Initialize touch if enabled
    if (hal_config.enable_touch)
    {
        g_hal->touch.init();
    }

    // Initialize LVGL
    g_hal->system.log(TAG, "Initialize LVGL library");
    lv_init();

    // Allocate draw buffers
    lv_color_t *buf1 = (lv_color_t *)g_hal->memory.malloc_dma(
        EXAMPLE_LCD_H_RES * EXAMPLE_LVGL_BUF_HEIGHT * sizeof(lv_color_t));
    if (!buf1)
    {
        fprintf(stderr, "Failed to allocate buffer 1\n");
#ifndef ESP_PLATFORM
        return -1;
#else
        return;
#endif
    }

    lv_color_t *buf2 = (lv_color_t *)g_hal->memory.malloc_dma(
        EXAMPLE_LCD_H_RES * EXAMPLE_LVGL_BUF_HEIGHT * sizeof(lv_color_t));
    if (!buf2)
    {
        fprintf(stderr, "Failed to allocate buffer 2\n");
        g_hal->memory.free(buf1);
#ifndef ESP_PLATFORM
        return -1;
#else
        return;
#endif
    }

    // Initialize LVGL draw buffers
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, EXAMPLE_LCD_H_RES * EXAMPLE_LVGL_BUF_HEIGHT);

    // Register display driver to LVGL
    g_hal->system.log(TAG, "Register display driver to LVGL");
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = EXAMPLE_LCD_H_RES;
    disp_drv.ver_res = EXAMPLE_LCD_V_RES;
#ifdef ESP_PLATFORM
    disp_drv.flush_cb = lvgl_flush_cb;
    disp_drv.rounder_cb = lvgl_rounder_cb;
    disp_drv.user_data = g_hal->display.get_display_handle();
#else
    disp_drv.flush_cb = sdl_display_flush;
#endif
    disp_drv.draw_buf = &disp_buf;
    lv_disp_t *disp = lv_disp_drv_register(&disp_drv);

    // Initialize default theme
    lv_theme_t *theme = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(disp, theme);
    g_hal->system.log(TAG, "Default theme initialized");

#ifdef ESP_PLATFORM
    // Install LVGL tick timer
    g_hal->system.log(TAG, "Install LVGL tick timer");
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &increase_lvgl_tick,
        .name = "lvgl_tick"};
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000));
#else
    // For native, we'll handle ticks in the main loop
    uint32_t last_tick = g_hal->system.get_tick_ms();
#endif

#if EXAMPLE_USE_TOUCH
#ifdef ESP_PLATFORM
    // Register touch input device for ESP32
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.disp = disp;
    indev_drv.read_cb = lvgl_touch_cb;
    lv_indev_drv_register(&indev_drv);
#else
    // Register SDL mouse input device for native
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.disp = disp;
    indev_drv.read_cb = sdl_mouse_read;
    lv_indev_drv_register(&indev_drv);
#endif
#endif

#ifdef ESP_PLATFORM
    // Create mutex and task for ESP32
    lvgl_mux = xSemaphoreCreateMutex();
    if (!lvgl_mux)
    {
        g_hal->system.log(TAG, "Failed to create LVGL mutex");
        return;
    }
    xTaskCreate(lvgl_port_task, "LVGL", EXAMPLE_LVGL_TASK_STACK_SIZE, NULL,
                EXAMPLE_LVGL_TASK_PRIORITY, NULL);
#else
    // Create thread for native
    pthread_t lvgl_thread;
    g_hal->system.log(TAG, "Creating LVGL thread");
    int ret = pthread_create(&lvgl_thread, NULL, lvgl_port_task, NULL);
    if (ret != 0)
    {
        fprintf(stderr, "Failed to create LVGL thread: %d\n", ret);
        return -1;
    }
    g_hal->system.log(TAG, "LVGL thread created");
    g_hal->system.delay_ms(100); // Give thread time to start
#endif

    // Display LVGL demo
    g_hal->system.log(TAG, "Display LVGL demos");
    g_hal->system.log(TAG, "About to lock for demo creation");
    if (lvgl_lock(-1))
    {
        g_hal->system.log(TAG, "Lock acquired, creating demo");

        // Debug: Check what color values we're creating
        lv_color_t white = lv_color_white();
        lv_color_t red = lv_color_make(255, 0, 0);
        lv_color_t black = lv_color_black();
        printf("[main] Color test: white=0x%04x red=0x%04x black=0x%04x\n",
               white.full, red.full, black.full);
        printf("[main] LV_COLOR_DEPTH=%d LV_COLOR_16_SWAP=%d\n",
               LV_COLOR_DEPTH, LV_COLOR_16_SWAP);

        // Create a full-screen object with red background
        lv_obj_t *obj = lv_obj_create(lv_scr_act());
        lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
        lv_obj_set_style_bg_color(obj, red, LV_PART_MAIN);
        lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN);
        lv_obj_set_style_radius(obj, 0, LV_PART_MAIN);
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

        g_hal->system.log(TAG, "Created red full-screen object");

        // Force immediate refresh
        lv_refr_now(NULL); // NULL uses the default display
        g_hal->system.log(TAG, "Forced immediate refresh");

        lvgl_unlock();
        g_hal->system.log(TAG, "Lock released");
    }

#ifdef ESP_PLATFORM
    // On ESP32, app_main returns and FreeRTOS continues running
#else
    // On native, keep the main thread running and update ticks
    while (1)
    {
        uint32_t current_tick = g_hal->system.get_tick_ms();
        if (current_tick - last_tick >= EXAMPLE_LVGL_TICK_PERIOD_MS)
        {
            lv_tick_inc(current_tick - last_tick);
            last_tick = current_tick;
        }
        g_hal->system.delay_ms(1);
    }

    pthread_join(lvgl_thread, NULL);
    return 0;
#endif
}
