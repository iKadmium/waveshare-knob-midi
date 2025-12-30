#ifndef STORAGE_SERVICE_H
#define STORAGE_SERVICE_H

#include "esp_err.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <string>
#include <memory>

class Parameter;
class Page;

/**
 * @brief Storage service for persisting MIDI parameter values using NVS
 */
class StorageService
{
public:
    StorageService();
    ~StorageService();

    /**
     * @brief Initialize NVS storage
     * @return ESP_OK on success
     */
    esp_err_t init();

    /**
     * @brief Save a parameter value to NVS
     * @param key Unique key for the parameter (e.g., "page1_param0")
     * @param value The value to save (0-127)
     * @return ESP_OK on success
     */
    esp_err_t saveParameterValue(const std::string& key, uint8_t value);

    /**
     * @brief Load a parameter value from NVS
     * @param key Unique key for the parameter
     * @param defaultValue Default value if key doesn't exist
     * @return The loaded value or defaultValue if not found
     */
    uint8_t loadParameterValue(const std::string& key, uint8_t defaultValue = 0);

    /**
     * @brief Save all parameters from a page
     * @param pageKey Unique key for the page (e.g., "page1")
     * @param page Shared pointer to the page
     * @return ESP_OK on success
     */
    esp_err_t savePage(const std::string& pageKey, std::shared_ptr<Page> page);

    /**
     * @brief Load all parameters for a page
     * @param pageKey Unique key for the page
     * @param page Shared pointer to the page
     * @return ESP_OK on success
     */
    esp_err_t loadPage(const std::string& pageKey, std::shared_ptr<Page> page);

    /**
     * @brief Clear all stored values
     * @return ESP_OK on success
     */
    esp_err_t clearAll();

private:
    nvs_handle_t nvsHandle_;
    bool initialized_;

    static constexpr const char* NVS_NAMESPACE = "midi_storage";
};

#endif // STORAGE_SERVICE_H
