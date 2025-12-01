#include <cstdio>
#include <cstring>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"

#include "blemidi.h"
#include "ui.h"
#include "attributes.h"
#include <vector>
#include <array>

#define TAG "KadmiumBLE"

// Define program names for instruments
static constexpr std::array<const char *, 6> tonex_programs = {
    "Clean",
    "Crunch",
    "Rhythm",
    "Lead",
    "Bass 1",
    "Bass 2",
};

static void task_midi(void *pvParameters)
{
    std::array<uint8_t, 3> message = {0x90, 0x3c, 0x7f};
    blemidi_send_message(0, message.data(), message.size());
}

void callback_midi_message_received(uint8_t blemidi_port, uint16_t timestamp, uint8_t midi_status, uint8_t *remaining_message, size_t len)
{
    ESP_LOGI(TAG, "Received MIDI message on port %d, timestamp %d, status 0x%02x, length %d",
             blemidi_port, timestamp, midi_status, len + 1);
}

void app_main()
{
    // Initialize MIDI attributes
    std::vector<attribute_t> midi_attributes;

    attribute_t volume;
    volume.title = "Volume";
    volume.type = ATTR_TYPE_CC;
    volume.channel = 0;
    volume.cc_number = 7;
    volume.value = 64;
    midi_attributes.push_back(volume);

    attribute_t pan;
    pan.title = "Pan";
    pan.type = ATTR_TYPE_CC;
    pan.channel = 0;
    pan.cc_number = 10;
    pan.value = 64;
    midi_attributes.push_back(pan);

    attribute_t tonex;
    tonex.title = "ToneX";
    tonex.type = ATTR_TYPE_PROGRAM_CHANGE;
    tonex.channel = 0;
    tonex.pc.program_names = const_cast<const char **>(tonex_programs.data());
    tonex.pc.num_programs = static_cast<uint8_t>(tonex_programs.size());
    tonex.value = 0;
    midi_attributes.push_back(tonex);

    // Initialize UI first
    int ui_status = ui_init();
    if (ui_status < 0)
    {
        ESP_LOGE(TAG, "UI initialization failed with status=%d", ui_status);
    }
    else
    {
        ESP_LOGI(TAG, "UI initialized successfully");

        // Configure attributes
        ui_set_attributes(midi_attributes);
        ESP_LOGI(TAG, "Configured %zu MIDI attributes", midi_attributes.size());

        // Start UI task
        xTaskCreate(ui_task, "ui_task", 4096, nullptr, 5, nullptr);
    }

    // Initialize BLE MIDI
    int status = blemidi_init(callback_midi_message_received);
    if (status < 0)
    {
        ESP_LOGE(BLEMIDI_TAG, "BLE MIDI Driver returned status=%d", status);
    }
    else
    {
        ESP_LOGI(BLEMIDI_TAG, "BLE MIDI Driver initialized successfully");
        xTaskCreate(task_midi, "task_midi", 4096, nullptr, 8, nullptr);
    }
}