# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

# Suppress CMake developer warnings (including ESP-IDF component validation warnings)
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON CACHE BOOL "" FORCE)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(waveshare-knob-midi)

# Disable warnings as errors for all managed components
idf_build_set_property(COMPILE_OPTIONS "-Wno-error" APPEND)

# Optionally suppress all warnings in managed components
idf_component_get_property(lvgl_lib lvgl__lvgl COMPONENT_LIB)
if(lvgl_lib)
    target_compile_options(${lvgl_lib} PRIVATE -w)
    target_compile_definitions(${lvgl_lib} PUBLIC LV_CONF_PATH="${CMAKE_CURRENT_LIST_DIR}/lvgl_conf/lv_conf.h")
endif()

idf_component_get_property(blemidi_lib midibox_blemidi COMPONENT_LIB)
if(blemidi_lib)
    target_compile_options(${blemidi_lib} PRIVATE -w -Wno-implicit-function-declaration)
endif()