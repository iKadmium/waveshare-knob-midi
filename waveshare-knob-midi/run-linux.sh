#!/bin/bash
# Run script for Linux SDL backend

export LIBCLANG_PATH=/usr/lib/llvm-14/lib
export DEP_LV_CONFIG_PATH=/workspaces/waveshare-knob-midi/waveshare-knob-midi/lvgl_config
export RUST_LOG=${RUST_LOG:-info}

echo "Running SDL backend..."
cd sdl && cargo run "$@"
