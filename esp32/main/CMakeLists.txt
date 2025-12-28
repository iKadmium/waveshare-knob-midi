idf_component_register(SRCS "main.cpp" "ui.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES midibox_blemidi)

# Disable warnings for your component (if needed)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-error)
