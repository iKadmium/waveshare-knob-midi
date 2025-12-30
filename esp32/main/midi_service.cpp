#include "midi_service.h"
#include "blemidi.h"
#include "esp_log.h"
#include "esp_log_buffer.h"
#include <string.h>

static const char* TAG = "MidiService";

// Static callback for BLE MIDI received messages
static void midi_message_received_callback(uint8_t blemidi_port, uint16_t timestamp,
    uint8_t midi_status, uint8_t* remaining_message,
    size_t len, size_t continued_sysex_pos)
{
    ESP_LOGI(TAG, "Received MIDI: port=%d, timestamp=%d, status=0x%02x, len=%d",
        blemidi_port, timestamp, midi_status, len);
    if (len > 0 && remaining_message != nullptr)
    {
        ESP_LOG_BUFFER_HEX(TAG, remaining_message, len);
    }

    if (midi_status == 0xF0 && len >= 4 &&
        remaining_message[0] == 0x7E &&  // Universal Non-Real Time
        remaining_message[2] == 0x06 &&  // General Information
        remaining_message[3] == 0x01)    // Identity Request
    {
        ESP_LOGI(TAG, "Received Universal Identity Request - sending reply");

        // MIDI Identity Reply: F0 7E <device> 06 02 <manufacturer> <family> <member> <version> F7
        uint8_t identity_reply[] = {
            0xF0,       // SysEx start
            0x7E,       // Universal Non-Real Time
            0x7F,       // Device ID (all devices)
            0x06,       // General Information
            0x02,       // Identity Reply
            0x00, 0x20, 0x6B,  // Manufacturer ID (3 bytes - generic/experimental)
            0x00, 0x01, // Device family (generic controller)
            0x00, 0x01, // Device family member
            0x01, 0x00, 0x00, 0x00, // Software version (1.0.0.0)
            0xF7        // SysEx end
        };

        blemidi_send_message(blemidi_port, identity_reply, sizeof(identity_reply));
    }
}

MidiService::MidiService() : initialized_(false)
{
}

esp_err_t MidiService::init()
{
    if (initialized_)
    {
        ESP_LOGW(TAG, "MIDI service already initialized");
        return ESP_OK;
    }

    int32_t status = blemidi_init((void*) midi_message_received_callback);
    if (status < 0)
    {
        ESP_LOGE(TAG, "BLE MIDI driver failed to initialize, status=%d", status);
        return ESP_FAIL;
    }

    initialized_ = true;
    ESP_LOGI(TAG, "BLE MIDI service initialized successfully");
    return ESP_OK;
}

void MidiService::sendCC(uint8_t channel, uint8_t ccNumber, uint8_t value)
{
    if (!initialized_)
    {
        ESP_LOGW(TAG, "Cannot send CC: MIDI service not initialized");
        return;
    }

    // Ensure values are within valid range
    channel &= 0x0F;    // 4 bits (0-15)
    ccNumber &= 0x7F;   // 7 bits (0-127)
    value &= 0x7F;      // 7 bits (0-127)

    // MIDI CC message: [Status (0xB0 | channel), CC number, value]
    uint8_t message[3] = {
        static_cast<uint8_t>(0xB0 | channel),
        ccNumber,
        value
    };

    int32_t result = blemidi_send_message(0, message, sizeof(message));
    if (result < 0)
    {
        ESP_LOGE(TAG, "Failed to send CC message, result=%d", result);
    }
    else
    {
        ESP_LOGI(TAG, "Sent CC: channel=%d, cc=%d, value=%d", channel, ccNumber, value);
    }
}

void MidiService::sendProgramChange(uint8_t channel, uint8_t program)
{
    if (!initialized_)
    {
        ESP_LOGW(TAG, "Cannot send Program Change: MIDI service not initialized");
        return;
    }

    // Ensure values are within valid range
    channel &= 0x0F;  // 4 bits (0-15)
    program &= 0x7F;  // 7 bits (0-127)

    // MIDI Program Change message: [Status (0xC0 | channel), program]
    uint8_t message[2] = {
        static_cast<uint8_t>(0xC0 | channel),
        program
    };

    int32_t result = blemidi_send_message(0, message, sizeof(message));
    if (result < 0)
    {
        ESP_LOGE(TAG, "Failed to send Program Change message, result=%d", result);
    }
    else
    {
        ESP_LOGI(TAG, "Sent Program Change: channel=%d, program=%d", channel, program);
    }
}

void MidiService::sendParameter(std::shared_ptr<Parameter> param)
{
    if (!param)
    {
        ESP_LOGW(TAG, "Cannot send null parameter");
        return;
    }

    switch (param->getType())
    {
    case ParameterType::CC:
    {
        // Use static_cast since we know the type from getType()
        auto ccParam = std::static_pointer_cast<CCParameter>(param);
        if (ccParam)
        {
            sendCC(ccParam->getChannel(), ccParam->getCCNumber(), ccParam->getValue());
        }
        break;
    }
    case ParameterType::BOOLEAN_CC:
    {
        // Boolean CC also sends as a regular CC message (value is either 0 or 127)
        auto boolParam = std::static_pointer_cast<BooleanCCParameter>(param);
        if (boolParam)
        {
            sendCC(boolParam->getChannel(), boolParam->getCCNumber(), boolParam->getValue());
        }
        break;
    }
    case ParameterType::PROGRAM_CHANGE:
    {
        sendProgramChange(param->getChannel(), param->getValue());
        break;
    }
    default:
        ESP_LOGW(TAG, "Unknown parameter type");
        break;
    }
}

void MidiService::tick()
{
    if (initialized_)
    {
        blemidi_tick();
    }
}

bool MidiService::isConnected() const
{
    if (!initialized_)
    {
        return false;
    }
    return blemidi_is_connected() != 0;
}
