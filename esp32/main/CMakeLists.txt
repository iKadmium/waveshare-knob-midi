idf_component_register(
    SRCS "main.cpp" "display_touch.cpp" "ui_components.cpp" "midi_service.cpp" "storage_service.cpp"
    INCLUDE_DIRS "."
    PRIV_REQUIRES driver
    REQUIRES user_encoder_bsp i2c_bsp lcd_touch_bsp lcd_bl_pwm_bsp blemidi nvs_flash)

set_source_files_properties(
    ${LV_DEMOS_SOURCES}
    PROPERTIES COMPILE_OPTIONS
    -DLV_LVGL_H_INCLUDE_SIMPLE)

# Add C++ specific flags to allow legacy code compatibility
target_compile_options(${COMPONENT_LIB} PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-fpermissive>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-missing-field-initializers>
)
