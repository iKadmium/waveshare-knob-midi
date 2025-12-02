#!/bin/bash
# Build script for Linux target with LVGL and SDL support

export LIBCLANG_PATH=/usr/lib/llvm-14/lib
export DEP_LV_CONFIG_PATH=/workspaces/waveshare-knob-midi/waveshare-knob-midi/lvgl_config

echo "Building SDL backend for Linux..."
cd sdl && cargo build "$@"
