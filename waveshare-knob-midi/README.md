# Waveshare Knob MIDI

Multi-platform MIDI controller firmware with LVGL UI support.

## Features

- **Dual-target builds**: Linux (SDL) and ESP32-S3
- **Workspace architecture**: Separate toolchains per backend
- **LVGL UI**: Full UI development on Linux with SDL2
- **Shared core**: Business logic shared across platforms

## Quick Start

### Linux/SDL Development

```bash
./run-linux.sh
```

Fast iteration, UI testing without hardware.

### ESP32 Hardware

```bash
./build-esp32.sh
cd esp32 && cargo run
```

Flash and monitor on real hardware.

## Project Structure

- **`core/`** - Shared business logic (platform-agnostic)
- **`sdl/`** - Linux backend with LVGL + SDL2 (stable toolchain)
- **`esp32/`** - ESP32-S3 backend with ESP-IDF (ESP toolchain)
- **`lvgl_config/`** - LVGL configuration files

Each backend crate has its own `rust-toolchain.toml`, eliminating toolchain conflicts!

## Documentation

See [BUILD.md](BUILD.md) for detailed build instructions and architecture explanation.

## Development Workflow

1. Write shared logic in `core/src/lib.rs`
2. Test UI on Linux: `./run-linux.sh` (fast iteration)
3. Test on hardware: `cd esp32 && cargo run`

## Requirements

- Rust (stable + ESP toolchains)
- SDL2 development libraries
- ESP-IDF v5.3.3 (auto-managed)

## License

TBD
