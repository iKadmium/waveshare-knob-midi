/**
 * @file lv_drv_conf.h
 * Configuration file for v8.3.0
 */

#ifndef LV_DRV_CONF_H
#define LV_DRV_CONF_H

#include "lv_conf.h"

/*********************
 * DISPLAY INTERFACE
 *********************/

/* SDL based drivers for display, mouse, mousewheel and keyboard*/
#ifndef USE_SDL
#define USE_SDL 1
#endif

/* Hardware accelerated SDL driver */
#ifndef USE_SDL_GPU
#define USE_SDL_GPU 0
#endif

#if USE_SDL || USE_SDL_GPU

#ifndef SDL_INCLUDE_PATH
#define SDL_INCLUDE_PATH <SDL2/SDL.h>
#endif
#include SDL_INCLUDE_PATH

#define SDL_HOR_RES 360
#define SDL_VER_RES 360

/* Scale window by this factor (useful when simulating small screens) */
#define SDL_ZOOM 2

/* Used to test true double buffering with only address changing.
 * Set LV_VDB_SIZE = (LV_HOR_RES * LV_VER_RES) and  LV_VDB_DOUBLE = 1 and LV_COLOR_DEPTH = 32" */
#define SDL_DOUBLE_BUFFERED 0

/*Open two windows to test multi display support*/
#define SDL_DUAL_DISPLAY 0
#endif

/* Disable all other display drivers */
#define USE_MONITOR 0
#define USE_WINDOWS 0
#define USE_GTK 0
#define USE_SSD1963 0
#define USE_R61581 0
#define USE_ST7565 0
#define USE_UC1610 0
#define USE_SHARP_MIP 0
#define USE_ILI9341 0

/*********************
 * INPUT DEVICES INTERFACE
 *********************/

/* SDL based drivers for mouse, mousewheel and keyboard*/
#define USE_SDL_MOUSE USE_SDL
#define USE_SDL_MOUSEWHEEL USE_SDL
#define USE_SDL_KEYBOARD USE_SDL

/* Disable all other input drivers */
#define USE_MOUSE 0
#define USE_MOUSEWHEEL 0
#define USE_KEYBOARD 0
#define USE_EVDEV 0
#define USE_XKB 0
#define USE_LIBINPUT 0
#define USE_BSD_FBDEV 0
#define USE_XPT2046 0
#define USE_FT5406EE8 0
#define USE_AD_TOUCH 0
#define USE_ENCODER 0
#define USE_BUTTON 0

#endif /*LV_DRV_CONF_H*/
