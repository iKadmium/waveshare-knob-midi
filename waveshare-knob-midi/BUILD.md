# Building the Project

This project uses a Cargo workspace with separate crates for different backends, allowing each to use its own toolchain.

## ✅ What Works

- **SDL Backend (Linux) + LVGL**: Full UI development and testing ✅
- **ESP32 Backend (without LVGL)**: Core functionality builds and runs ✅
- **Separate toolchains per crate**: No more toolchain conflicts! ✅

## Project Structure

```
waveshare-knob-midi/
├── Cargo.toml           # Workspace definition
├── core/                # Shared business logic
│   ├── Cargo.toml
│   └── src/lib.rs
├── esp32/               # ESP32-S3 backend
│   ├── .cargo/config.toml
│   ├── rust-toolchain.toml  # ESP toolchain
│   ├── Cargo.toml
│   ├── build.rs
│   └── src/main.rs
├── sdl/                 # SDL/Linux backend  
│   ├── rust-toolchain.toml  # Stable toolchain
│   ├── Cargo.toml
│   └── src/main.rs
├── lvgl_config/         # Shared LVGL configuration
├── build-linux.sh       # Build SDL backend
├── build-esp32.sh       # Build ESP32 backend
└── run-linux.sh         # Run SDL backend
```

## Prerequisites

- Rust toolchains:
  - `stable` (automatically used by `sdl/` crate)
  - `esp` (automatically used by `esp32/` crate)
- SDL2: `sudo apt-get install libsdl2-dev`
- ESP-IDF v5.3.3 (auto-managed by `embuild`)

## Quick Start

### Build & Run SDL Backend (Linux)

```bash
./run-linux.sh
```

Or build only:
```bash
./build-linux.sh
```

### Build ESP32 Backend

```bash
./build-esp32.sh
```

Flash to device:
```bash
cd esp32 && cargo run
# or
cd esp32 && cargo espflash flash --monitor
```

## How It Works

### Workspace Architecture

The **workspace** pattern solves the toolchain conflict:

1. **Root `Cargo.toml`**: Defines workspace members and shared dependencies
2. **`core/`**: Platform-agnostic library with shared business logic
3. **`esp32/`**: Binary crate with `rust-toolchain.toml` → ESP toolchain
4. **`sdl/`**: Binary crate with `rust-toolchain.toml` → Stable toolchain

When you `cd` into a crate directory, Rust automatically uses that crate's toolchain!

### Dependencies

**Core (shared):**
- `log` - Logging interface

**ESP32 Backend:**
- `waveshare-knob-midi-core` - Shared logic
- `esp-idf-svc` - ESP-IDF bindings
- LVGL currently disabled (bindgen issues)

**SDL Backend:**
- `waveshare-knob-midi-core` - Shared logic  
- `env_logger` - Logging implementation
- `embedded-graphics` - Graphics primitives
- `lvgl` with SDL drivers + embedded-graphics

### Build Scripts

Scripts handle environment variables and directory navigation:

**`build-linux.sh`:**
- Sets `LIBCLANG_PATH` for bindgen
- Sets `DEP_LV_CONFIG_PATH` for LVGL
- Builds in `sdl/` directory (uses stable toolchain)

**`build-esp32.sh`:**
- Sets ESP clang path
- Builds in `esp32/` directory (uses ESP toolchain)

**`run-linux.sh`:**
- Sets up environment and runs SDL binary

## Development Workflow

1. **Shared code** goes in `core/src/lib.rs`
2. **Platform-specific initialization** in respective `main.rs` files
3. **Test UI on Linux**: `./run-linux.sh` (fast iteration)
4. **Test on hardware**: `cd esp32 && cargo run` (or use espflash)

## LVGL Configuration

Shared configuration in `lvgl_config/`:
- `lv_conf.h` - 64KB memory, RGB565, all widgets
- `lv_drv_conf.h` - SDL2 @ 240x240, 2x zoom

## Known Issues

### ESP32 + LVGL

The `lvgl-sys` bindgen cross-compilation issue persists, but with the workspace structure, we can now:

1. **Keep developing** - LVGL works perfectly on SDL backend
2. **Test independently** - Each backend has isolated dependencies
3. **Future fix** - Can add LVGL to ESP32 once bindgen issues are resolved

The workspace architecture makes it easier to experiment with solutions (custom lvgl-sys fork, build script patches, etc.) without affecting the working SDL backend.

## Manual Commands

If you prefer to work without scripts:

### SDL Backend
```bash
cd sdl
export LIBCLANG_PATH=/usr/lib/llvm-14/lib
export DEP_LV_CONFIG_PATH=$(pwd)/../lvgl_config
cargo build
RUST_LOG=info cargo run
```

### ESP32 Backend
```bash
cd esp32  
cargo build  # Automatically uses ESP toolchain
cargo run    # Flash and monitor
```

### Build Entire Workspace
```bash
# From workspace root
cargo build --workspace  # Builds all crates
```

## Adding New Backends

To add a new backend (e.g., WASM, bare-metal ARM):

1. Create new crate: `cargo new --bin <name>`
2. Add to workspace members in root `Cargo.toml`
3. Add `rust-toolchain.toml` with appropriate toolchain
4. Depend on `waveshare-knob-midi-core`
5. Implement platform-specific initialization

## Benefits of Workspace Approach

✅ **No toolchain conflicts** - Each crate specifies its own toolchain  
✅ **Shared code** - Business logic in `core/` used by all backends  
✅ **Independent builds** - Backends don't interfere with each other  
✅ **Easy testing** - Run SDL backend without ESP toolchain  
✅ **Scalable** - Easy to add more backends (WASM, embedded, etc.)  
✅ **Clean separation** - Platform code isolated from business logic
