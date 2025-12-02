fn main() {
    // Initialize ESP-IDF system
    esp_idf_svc::sys::link_patches();
    esp_idf_svc::log::EspLogger::initialize_default();

    log::info!("ESP32 backend starting");
    
    waveshare_knob_midi_core::init();
    waveshare_knob_midi_core::run();
    
    log::info!("ESP32 backend running");
}
