# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(LVGL_Test)

# Add global include directory for user_config.h
idf_build_set_property(INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include" APPEND)
