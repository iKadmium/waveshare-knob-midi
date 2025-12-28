#include "hal.h"
#include <stdio.h>

const hal_ops_t *g_hal = NULL;

int hal_init(void)
{
#ifdef ESP_PLATFORM
    g_hal = hal_esp32_init();
#else
    g_hal = hal_native_init();
#endif

    if (!g_hal)
    {
        fprintf(stderr, "Failed to initialize HAL\n");
        return -1;
    }

    return 0;
}
