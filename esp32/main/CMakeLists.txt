idf_component_register(
    SRCS "main.cpp" "display_touch.cpp"
    INCLUDE_DIRS ".")

set_source_files_properties(
    ${LV_DEMOS_SOURCES}
    PROPERTIES COMPILE_OPTIONS
    -DLV_LVGL_H_INCLUDE_SIMPLE)

# Add C++ specific flags to allow legacy code compatibility
target_compile_options(${COMPONENT_LIB} PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-fpermissive>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-missing-field-initializers>
)
