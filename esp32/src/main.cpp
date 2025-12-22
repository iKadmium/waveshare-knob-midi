#ifdef ARDUINO
// Arduino framework (ESP32)
#include <Arduino.h>

void setup()
{
    Serial.begin(115200);
    Serial.println("Running on ESP32 with Arduino");
}

void loop()
{
    // Main loop
    delay(1000);
}

#else
// Native platform
#include <iostream>

int main(void)
{
    std::cout << "Running on native platform\n";
    return 0;
}
#endif