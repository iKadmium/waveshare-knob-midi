fn main() {
    // Check if we're building for ESP-IDF target  
    #[cfg(target_os = "espidf")]
    {
        embuild::espidf::sysenv::output();
    }
}
