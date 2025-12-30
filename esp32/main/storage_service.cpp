#include "storage_service.h"
#include "midi_model.h"
#include "esp_log.h"
#include <sstream>

static const char* TAG = "StorageService";

StorageService::StorageService()
    : nvsHandle_(0), initialized_(false)
{
}

StorageService::~StorageService()
{
    if (initialized_)
    {
        nvs_close(nvsHandle_);
    }
}

esp_err_t StorageService::init()
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // NVS partition was truncated and needs to be erased
        ESP_LOGW(TAG, "NVS partition needs to be erased");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize NVS flash: %s", esp_err_to_name(ret));
        return ret;
    }

    // Open NVS handle
    ret = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvsHandle_);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to open NVS namespace: %s", esp_err_to_name(ret));
        return ret;
    }

    initialized_ = true;
    ESP_LOGI(TAG, "Storage service initialized successfully");
    return ESP_OK;
}

esp_err_t StorageService::saveParameterValue(const std::string& key, uint8_t value)
{
    if (!initialized_)
    {
        ESP_LOGE(TAG, "Storage service not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret = nvs_set_u8(nvsHandle_, key.c_str(), value);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to save parameter %s: %s", key.c_str(), esp_err_to_name(ret));
        return ret;
    }

    // Commit written value
    ret = nvs_commit(nvsHandle_);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to commit NVS changes: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGD(TAG, "Saved parameter %s = %d", key.c_str(), value);
    return ESP_OK;
}

uint8_t StorageService::loadParameterValue(const std::string& key, uint8_t defaultValue)
{
    if (!initialized_)
    {
        ESP_LOGW(TAG, "Storage service not initialized, returning default value");
        return defaultValue;
    }

    uint8_t value = defaultValue;
    esp_err_t ret = nvs_get_u8(nvsHandle_, key.c_str(), &value);

    if (ret == ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGD(TAG, "Parameter %s not found, using default value %d", key.c_str(), defaultValue);
        return defaultValue;
    }
    else if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to load parameter %s: %s", key.c_str(), esp_err_to_name(ret));
        return defaultValue;
    }

    ESP_LOGD(TAG, "Loaded parameter %s = %d", key.c_str(), value);
    return value;
}

esp_err_t StorageService::savePage(const std::string& pageKey, std::shared_ptr<Page> page)
{
    if (!page)
    {
        ESP_LOGE(TAG, "Invalid page pointer");
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(TAG, "Saving page '%s' with %d parameters", pageKey.c_str(), page->getParameterCount());

    // Save each parameter value
    for (size_t i = 0; i < page->getParameterCount(); i++)
    {
        auto param = page->getParameter(i);
        if (param)
        {
            std::stringstream ss;
            ss << pageKey << "_p" << i;
            std::string key = ss.str();

            esp_err_t ret = saveParameterValue(key, param->getValue());
            if (ret != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to save parameter %d of page '%s'", i, pageKey.c_str());
                return ret;
            }
        }
    }

    ESP_LOGI(TAG, "Successfully saved page '%s'", pageKey.c_str());
    return ESP_OK;
}

esp_err_t StorageService::loadPage(const std::string& pageKey, std::shared_ptr<Page> page)
{
    if (!page)
    {
        ESP_LOGE(TAG, "Invalid page pointer");
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(TAG, "Loading page '%s' with %d parameters", pageKey.c_str(), page->getParameterCount());

    // Load each parameter value
    for (size_t i = 0; i < page->getParameterCount(); i++)
    {
        auto param = page->getParameter(i);
        if (param)
        {
            std::stringstream ss;
            ss << pageKey << "_p" << i;
            std::string key = ss.str();

            uint8_t value = loadParameterValue(key, param->getValue());
            param->setValue(value);

            ESP_LOGD(TAG, "Loaded parameter %d (%s) = %d", i, param->getName().c_str(), value);
        }
    }

    ESP_LOGI(TAG, "Successfully loaded page '%s'", pageKey.c_str());
    return ESP_OK;
}

esp_err_t StorageService::clearAll()
{
    if (!initialized_)
    {
        ESP_LOGE(TAG, "Storage service not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret = nvs_erase_all(nvsHandle_);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to erase NVS: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = nvs_commit(nvsHandle_);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to commit NVS changes: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "Cleared all stored values");
    return ESP_OK;
}
