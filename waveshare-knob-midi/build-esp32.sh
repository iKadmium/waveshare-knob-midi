#!/bin/bash
# Build script for ESP32-S3 target with LVGL support

export LIBCLANG_PATH="/home/esp/.rustup/toolchains/esp/xtensa-esp32-elf-clang/esp-19.1.2_20250225/esp-clang/lib"
export DEP_LV_CONFIG_PATH=/workspaces/waveshare-knob-midi/waveshare-knob-midi/lvgl_config

echo "Building ESP32 backend..."
cd esp32 && cargo build "$@"
