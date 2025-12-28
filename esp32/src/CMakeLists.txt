idf_component_register(
    SRCS "main.cpp" "hal.cpp" "hal_esp32.cpp"
    INCLUDE_DIRS "."
    REQUIRES driver i2c_bsp lcd_touch_bsp lcd_bl_pwm_bsp lvgl)

set_source_files_properties(
    ${LV_DEMOS_SOURCES}
    PROPERTIES COMPILE_OPTIONS
    -DLV_LVGL_H_INCLUDE_SIMPLE)
