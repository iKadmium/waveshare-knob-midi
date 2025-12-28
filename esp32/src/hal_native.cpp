#include "hal.h"

#ifndef ESP_PLATFORM

#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <pthread.h>
#include "lvgl.h"
#include "sdl/sdl.h"

static const char *TAG = "hal_native";

// Display state
static bool s_initialized = false;
static lv_disp_t *s_disp = NULL;

// Tick thread state
static pthread_t s_tick_thread;
static bool s_tick_thread_running = false;

// Input device handles
static lv_indev_t *s_mouse_indev = NULL;

// Tick thread function
static void *tick_thread(void *data)
{
    (void)data;

    while (s_tick_thread_running)
    {
        usleep(5000);
        lv_tick_inc(5); // Tell LVGL that 5 milliseconds elapsed
    }

    return NULL;
}

// Display HAL implementation
static int hal_native_display_init(hal_config_t *config)
{
    if (s_initialized)
    {
        return 0;
    }

    // Initialize SDL display through lv_drivers
    sdl_init();

    // Create display buffers
    static lv_disp_draw_buf_t disp_buf1;
    static lv_color_t *buf1_1 = NULL;
    static lv_color_t *buf1_2 = NULL;
    
    // Use config dimensions if available, otherwise use MONITOR_HOR_RES
    uint16_t hor_res = config ? config->screen_width : 360;
    uint16_t ver_res = config ? config->screen_height : 360;
    uint16_t buf_height = config ? config->buffer_height : 100;
    
    // Allocate buffers
    size_t buf_size = hor_res * buf_height;
    buf1_1 = (lv_color_t *)malloc(buf_size * sizeof(lv_color_t));
    buf1_2 = (lv_color_t *)malloc(buf_size * sizeof(lv_color_t));
    
    if (!buf1_1 || !buf1_2)
    {
        printf("[%s] Failed to allocate display buffers\n", TAG);
        if (buf1_1) free(buf1_1);
        if (buf1_2) free(buf1_2);
        return -1;
    }
    
    lv_disp_draw_buf_init(&disp_buf1, buf1_1, buf1_2, buf_size);

    // Create and register display driver
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &disp_buf1;
    disp_drv.flush_cb = sdl_display_flush;
    disp_drv.hor_res = hor_res;
    disp_drv.ver_res = ver_res;
    disp_drv.antialiasing = 1;

    s_disp = lv_disp_drv_register(&disp_drv);

    // Set up theme
    lv_theme_t *th = lv_theme_default_init(s_disp, 
        lv_palette_main(LV_PALETTE_BLUE), 
        lv_palette_main(LV_PALETTE_RED), 
        LV_THEME_DEFAULT_DARK, 
        LV_FONT_DEFAULT);
    lv_disp_set_theme(s_disp, th);

    // Start tick thread
    s_tick_thread_running = true;
    pthread_create(&s_tick_thread, NULL, tick_thread, NULL);

    s_initialized = true;
    return 0;
}

static void hal_native_display_deinit(void)
{
    if (!s_initialized)
    {
        return;
    }

    // Stop tick thread
    s_tick_thread_running = false;
    pthread_join(s_tick_thread, NULL);

    s_initialized = false;
}

static hal_display_handle_t hal_native_get_display_handle(void)
{
    return (hal_display_handle_t)s_disp;
}

static void hal_native_set_backlight(uint16_t duty)
{
    // No-op for SDL
    (void)duty;
}

static void hal_native_flush_display(int16_t x1, int16_t y1, int16_t x2, int16_t y2, void *color_map, size_t size)
{
    // No-op - LVGL calls sdl_display_flush directly via display driver
    (void)x1;
    (void)y1;
    (void)x2;
    (void)y2;
    (void)color_map;
    (void)size;
}

// Touch HAL implementation
static int hal_native_touch_init(void)
{
    // Create default group for input devices
    lv_group_t *g = lv_group_create();
    lv_group_set_default(g);

    // Register mouse (pointer) input device
    static lv_indev_drv_t indev_drv_mouse;
    lv_indev_drv_init(&indev_drv_mouse);
    indev_drv_mouse.type = LV_INDEV_TYPE_POINTER;
    indev_drv_mouse.read_cb = sdl_mouse_read;
    s_mouse_indev = lv_indev_drv_register(&indev_drv_mouse);

    // Register keyboard input device
    static lv_indev_drv_t indev_drv_kb;
    lv_indev_drv_init(&indev_drv_kb);
    indev_drv_kb.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv_kb.read_cb = sdl_keyboard_read;
    lv_indev_t *kb_indev = lv_indev_drv_register(&indev_drv_kb);
    lv_indev_set_group(kb_indev, g);

    // Register mousewheel (encoder) input device
    static lv_indev_drv_t indev_drv_encoder;
    lv_indev_drv_init(&indev_drv_encoder);
    indev_drv_encoder.type = LV_INDEV_TYPE_ENCODER;
    indev_drv_encoder.read_cb = sdl_mousewheel_read;
    lv_indev_t *enc_indev = lv_indev_drv_register(&indev_drv_encoder);
    lv_indev_set_group(enc_indev, g);

    // Set up mouse cursor after display is ready
    if (s_mouse_indev && s_disp)
    {
        LV_IMG_DECLARE(mouse_cursor_icon);
        lv_obj_t *cursor_obj = lv_img_create(lv_scr_act());
        lv_img_set_src(cursor_obj, &mouse_cursor_icon);
        lv_indev_set_cursor(s_mouse_indev, cursor_obj);
    }

    return 0;
}

static void hal_native_touch_deinit(void)
{
    // No-op
}

static bool hal_native_read_touch(uint16_t *x, uint16_t *y)
{
    // No-op - LVGL calls sdl_mouse_read directly via input driver
    (void)x;
    (void)y;
    return false;
}

// System HAL implementation
static void hal_native_delay_ms(uint32_t ms)
{
    usleep(ms * 1000);
}

static uint32_t hal_native_get_tick_ms(void)
{
    return lv_tick_get();
}

static void hal_native_log(const char *tag, const char *format, ...)
{
    printf("[%s] ", tag);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}

// Memory HAL implementation
static void *hal_native_malloc_dma(size_t size)
{
    return malloc(size);
}

static void hal_native_free(void *ptr)
{
    if (ptr)
    {
        free(ptr);
    }
}

// HAL operations structure
static const hal_ops_t native_hal_ops = {
    .display = {
        .init = hal_native_display_init,
        .deinit = hal_native_display_deinit,
        .get_display_handle = hal_native_get_display_handle,
        .set_backlight = hal_native_set_backlight,
        .flush_display = hal_native_flush_display,
    },
    .touch = {
        .init = hal_native_touch_init,
        .deinit = hal_native_touch_deinit,
        .read_touch = hal_native_read_touch,
    },
    .system = {
        .delay_ms = hal_native_delay_ms,
        .get_tick_ms = hal_native_get_tick_ms,
        .log = hal_native_log,
    },
    .memory = {
        .malloc_dma = hal_native_malloc_dma,
        .free = hal_native_free,
    },
};

const hal_ops_t *hal_native_init(void)
{
    return &native_hal_ops;
}

#endif // !ESP_PLATFORM
