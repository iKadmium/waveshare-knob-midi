fn main() {
    env_logger::init();
    
    log::info!("SDL backend starting");
    
    waveshare_knob_midi_core::init();
    waveshare_knob_midi_core::run();
    
    log::info!("SDL backend running");
}
