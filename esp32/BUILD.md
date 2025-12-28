# Waveshare Knob MIDI - Build Guide

This project now supports both ESP32 hardware and native Linux builds with an Hardware Abstraction Layer (HAL).

## Architecture

The project has been refactored to C++ with a HAL that abstracts:
- **Display operations**: LCD initialization, backlight control, framebuffer flushing
- **Touch input**: Touch screen or mouse input
- **System functions**: Delays, timing, logging
- **Memory allocation**: DMA-capable memory on ESP32, regular malloc on native

The HAL automatically selects the appropriate implementation based on the target platform.

## Building for ESP32-S3

### Prerequisites
- PlatformIO installed
- ESP32-S3 DevKit connected via USB

### Build and Upload
```bash
cd esp32
pio run -e esp32-s3-idf -t upload
```

### Monitor Serial Output
```bash
pio device monitor -e esp32-s3-idf
```

## Building for Native Linux

### Prerequisites
Install SDL2 development libraries:

**Debian/Ubuntu:**
```bash
sudo apt-get install libsdl2-dev
```

**Fedora:**
```bash
sudo dnf install SDL2-devel
```

**Arch Linux:**
```bash
sudo pacman -S sdl2
```

### Build and Run

#### Using CMake (Recommended)
```bash
cd esp32
mkdir -p build_native
cd build_native
cmake -DCMAKE_BUILD_TYPE=Debug -f ../CMakeLists.native.txt ..
make
./bin/waveshare_knob_native
```

#### Using Make
```bash
cd esp32
make -f Makefile.native
./native_build
```

#### Using PlatformIO
```bash
cd esp32
pio run -e native -t exec
```

## Project Structure

```
esp32/
├── src/
│   ├── main.cpp          # Main application (platform-agnostic)
│   ├── hal.h             # HAL interface definitions
│   ├── hal.cpp           # HAL initialization
│   ├── hal_esp32.cpp     # ESP32-specific HAL implementation
│   └── hal_native.cpp    # Linux/SDL2 HAL implementation
├── include/
│   ├── lv_conf.h         # LVGL configuration
│   └── user_config.h     # Hardware pin definitions
├── components/           # ESP32 BSP components
├── managed_components/   # ESP-IDF managed components (LVGL, LCD driver)
├── platformio.ini        # PlatformIO configuration
├── CMakeLists.txt        # ESP-IDF CMake config
├── CMakeLists.native.txt # Native build CMake config
└── Makefile.native       # Simple Make-based native build
```

## HAL API Overview

### Display Operations
- `display.init()` - Initialize display hardware/SDL window
- `display.set_backlight()` - Control backlight brightness
- `display.flush_display()` - Update screen with new framebuffer data

### Touch Input
- `touch.init()` - Initialize touch controller
- `touch.read_touch()` - Read touch coordinates (or mouse on native)

### System Functions
- `system.delay_ms()` - Platform-appropriate delay
- `system.get_tick_ms()` - Get millisecond tick count
- `system.log()` - Platform-appropriate logging

### Memory Functions
- `memory.malloc_dma()` - Allocate DMA-capable memory (ESP32) or regular memory (native)
- `memory.free()` - Free allocated memory

## Native Build Features

The native build provides:
- SDL2 window displaying the 360x360 screen (scaled 2x for visibility)
- Mouse input simulating touch screen
- Real-time rendering of LVGL UI
- Useful for rapid UI development without hardware

## Development Workflow

1. **Develop UI on native**: Fast iteration without hardware
   ```bash
   make -f Makefile.native run
   ```

2. **Test on hardware**: Deploy to ESP32-S3
   ```bash
   pio run -e esp32-s3-idf -t upload -t monitor
   ```

3. **Debug native build**: Use standard Linux debugging tools
   ```bash
   gdb ./native_build
   ```

## Switching Between Targets

The code automatically detects the target platform using `#ifdef ESP_PLATFORM`:
- When `ESP_PLATFORM` is defined → ESP32 HAL is used
- When not defined → Native Linux HAL is used

No code changes needed to switch between targets!

## Known Limitations

### Native Build
- No haptic feedback simulation
- No I2C device simulation
- Display rotation not fully tested
- Some ESP32-specific LCD optimizations not applied

### ESP32 Build
- Requires physical hardware
- Slower iteration time compared to native

## Troubleshooting

### Native Build Issues

**SDL2 not found:**
```bash
# Verify SDL2 installation
pkg-config --cflags --libs sdl2
```

**LVGL demos not compiling:**
Ensure `lv_conf.h` has demos enabled:
```c
#define LV_USE_DEMO_WIDGETS 1
```

### ESP32 Build Issues

**Upload failed:**
- Check USB connection
- Verify correct port in `platformio.ini`
- Press BOOT button on ESP32-S3 during upload

**Build errors:**
```bash
# Clean and rebuild
pio run -e esp32-s3-idf -t clean
pio run -e esp32-s3-idf
```

## License

See main project LICENSE file.
