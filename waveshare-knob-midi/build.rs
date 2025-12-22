fn main() {
    // This sets up the ESP-IDF build environment
    embuild::espidf::sysenv::output();

    // If we're building for ESP32, we need to help lvgl-sys find the toolchain
    if std::env::var("TARGET")
        .unwrap_or_default()
        .contains("espidf")
    {
        // After esp-idf-sys builds, it will have set up the toolchain
        // We need to wait for it and export its compiler for lvgl-sys
        if let Ok(idf_path) = std::env::var("IDF_PATH") {
            println!("cargo:warning=Using ESP-IDF from: {}", idf_path);
        }
    }
}
