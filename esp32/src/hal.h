#ifndef HAL_H
#define HAL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    // Forward declarations for platform-specific types
    typedef void *hal_display_handle_t;
    typedef void *hal_touch_handle_t;

    // HAL Configuration
    typedef struct
    {
        uint16_t screen_width;
        uint16_t screen_height;
        uint16_t buffer_height;
        bool enable_touch;
    } hal_config_t;

    // Display callback types
    typedef void (*hal_flush_cb_t)(void *drv, const void *area, void *color_map);
    typedef bool (*hal_flush_ready_cb_t)(void *panel_io, void *event_data, void *user_ctx);

    // HAL Display API
    typedef struct
    {
        int (*init)(hal_config_t *config);
        void (*deinit)(void);
        hal_display_handle_t (*get_display_handle)(void);
        void (*set_backlight)(uint16_t duty);
        void (*flush_display)(int16_t x1, int16_t y1, int16_t x2, int16_t y2, void *color_map, size_t size);
    } hal_display_ops_t;

    // HAL Touch API
    typedef struct
    {
        int (*init)(void);
        void (*deinit)(void);
        bool (*read_touch)(uint16_t *x, uint16_t *y);
    } hal_touch_ops_t;

    // HAL System API
    typedef struct
    {
        void (*delay_ms)(uint32_t ms);
        uint32_t (*get_tick_ms)(void);
        void (*log)(const char *tag, const char *format, ...);
    } hal_system_ops_t;

    // HAL Memory API
    typedef struct
    {
        void *(*malloc_dma)(size_t size);
        void (*free)(void *ptr);
    } hal_memory_ops_t;

    // Main HAL structure
    typedef struct
    {
        hal_display_ops_t display;
        hal_touch_ops_t touch;
        hal_system_ops_t system;
        hal_memory_ops_t memory;
    } hal_ops_t;

    // Global HAL instance
    extern const hal_ops_t *g_hal;

    // Platform-specific initialization functions
    const hal_ops_t *hal_esp32_init(void);
    const hal_ops_t *hal_native_init(void);

    // Generic HAL initialization (selects appropriate platform)
    int hal_init(void);

#ifdef __cplusplus
}
#endif

#endif // HAL_H
