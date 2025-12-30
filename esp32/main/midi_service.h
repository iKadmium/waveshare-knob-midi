#ifndef MIDI_SERVICE_H
#define MIDI_SERVICE_H

#include <stdint.h>
#include "esp_err.h"
#include "midi_model.h"
#include <memory>

/**
 * @brief MIDI Service for sending BLE MIDI messages
 */
class MidiService
{
public:
    MidiService();
    ~MidiService() = default;

    /**
     * @brief Initialize the BLE MIDI service
     * @return ESP_OK on success, error code otherwise
     */
    esp_err_t init();

    /**
     * @brief Send a CC (Control Change) message
     * @param channel MIDI channel (0-15)
     * @param ccNumber Control Change number (0-127)
     * @param value Control Change value (0-127)
     */
    void sendCC(uint8_t channel, uint8_t ccNumber, uint8_t value);

    /**
     * @brief Send a Program Change message
     * @param channel MIDI channel (0-15)
     * @param program Program number (0-127)
     */
    void sendProgramChange(uint8_t channel, uint8_t program);

    /**
     * @brief Send a parameter value
     * @param param Parameter to send
     */
    void sendParameter(std::shared_ptr<Parameter> param);

    /**
     * @brief This should be called periodically (e.g., every 15ms) for timestamp handling
     */
    void tick();

private:
    bool initialized_;
};

#endif // MIDI_SERVICE_H
