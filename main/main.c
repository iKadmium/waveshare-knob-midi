#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"

#include "blemidi.h"

#define TAG "KadmiumBLE"

static void task_midi(void *pvParameters)
{
    // TODO: more comfortable packet creation via special APIs
    uint8_t message[3] = {0x90, 0x3c, 0x7f};
    blemidi_send_packet(0, message, sizeof(message));
}

void callback_midi_message_received(uint8_t blemidi_port, uint16_t timestamp, uint8_t midi_status, uint8_t *remaining_message, size_t len)
{
    ESP_LOGI(TAG, "Received MIDI message on port %d, timestamp %d, status 0x%02x, length %d",
             blemidi_port, timestamp, midi_status, len + 1);
}

void app_main(void)
{
    int status = blemidi_init(callback_midi_message_received);
    if (status < 0)
    {
        ESP_LOGE(BLEMIDI_TAG, "BLE MIDI Driver returned status=%d", status);
    }
    else
    {
        ESP_LOGI(BLEMIDI_TAG, "BLE MIDI Driver initialized successfully");
        xTaskCreate(task_midi, "task_midi", 4096, NULL, 8, NULL);
    }
}