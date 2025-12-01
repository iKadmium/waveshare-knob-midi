#include "ui.h"
#include "lvgl.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string>
#include <format>

#define TAG "UI"

// UI element references
static lv_obj_t *screen = nullptr;
static lv_obj_t *title_label = nullptr;
static lv_obj_t *arc = nullptr;

// Attribute management
static std::vector<attribute_t> attributes = {};
static size_t attribute_count = 0;
static int current_attribute_index = 0;

// Forward declarations for callbacks
static void swipe_event_cb(lv_event_t *e);
static void update_display_from_attribute();

/**
 * @brief Initialize the LVGL display and create UI elements
 */
int ui_init()
{
    ESP_LOGI(TAG, "Initializing UI");

    // Initialize LVGL
    lv_init();

    // TODO: Initialize display driver
    // You'll need to configure this based on your hardware:
    // - Create display buffer
    // - Register display driver
    // - Register touch input driver (if applicable)

    // For now, create a basic screen
    screen = lv_screen_active();
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);

    // Create title label (centered at top)
    title_label = lv_label_create(screen);
    lv_label_set_text(title_label, "Attribute");
    lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_20, 0);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 20);

    // Create arc (270 degrees, centered)
    arc = lv_arc_create(screen);
    lv_obj_set_size(arc, 200, 200);
    lv_obj_center(arc);

    // Configure arc properties
    lv_arc_set_rotation(arc, 135);     // Rotate to start at bottom-left
    lv_arc_set_bg_angles(arc, 0, 270); // 270 degree arc
    lv_arc_set_range(arc, 0, 127);     // MIDI value range
    lv_arc_set_value(arc, 0);

    // Style the arc
    lv_obj_set_style_arc_width(arc, 15, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, 15, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, lv_color_hex(0x404040), LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc, lv_color_hex(0x00AAFF), LV_PART_INDICATOR);

    // Remove the knob (circular handle)
    lv_obj_set_style_bg_opa(arc, LV_OPA_TRANSP, LV_PART_KNOB);

    // Create value label in center of arc
    auto *value_label = lv_label_create(screen);
    lv_label_set_text(value_label, "0");
    lv_obj_set_style_text_color(value_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(value_label, &lv_font_montserrat_32, 0);
    lv_obj_center(value_label);

    // Store value label as arc user data for easy access
    lv_obj_set_user_data(arc, value_label);

    // Add swipe gesture detection to screen
    lv_obj_add_event_cb(screen, swipe_event_cb, LV_EVENT_GESTURE, nullptr);
    lv_obj_add_flag(screen, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

    ESP_LOGI(TAG, "UI initialized successfully");
    return 0;
}

/**
 * @brief Update display based on current attribute
 */
static void update_display_from_attribute()
{
    auto *attr = &attributes[current_attribute_index];
    ui_set_title(attr->title);
    ui_set_value(attr->value);

    // Update arc range based on attribute type
    if (arc != nullptr)
    {
        if (attr->type == ATTR_TYPE_PROGRAM_CHANGE)
        {
            // Set range to number of programs
            lv_arc_set_range(arc, 0, attr->pc.num_programs - 1);
            ESP_LOGI(TAG, "Displaying PC: %s (Ch:%d Programs:%d Val:%d)",
                     attr->title, attr->channel + 1, attr->pc.num_programs, attr->value);
        }
        else
        {
            // Set range to MIDI CC range
            lv_arc_set_range(arc, 0, 127);
            ESP_LOGI(TAG, "Displaying CC: %s (Ch:%d CC:%d Val:%d)",
                     attr->title, attr->channel + 1, attr->cc_number, attr->value);
        }
    }
}

/**
 * @brief Swipe gesture callback for attribute navigation
 */
static void swipe_event_cb(lv_event_t *e)
{
    auto dir = lv_indev_get_gesture_dir(lv_indev_active());

    if (dir == LV_DIR_LEFT)
    {
        ui_next_attribute();
    }
    else if (dir == LV_DIR_RIGHT)
    {
        ui_prev_attribute();
    }
}

/**
 * @brief Set the attributes array
 */
void ui_set_attributes(std::vector<attribute_t> &attrs)
{
    attributes.clear();
    for (const auto &attr : attrs)
    {
        attributes.push_back(attr);
    }
    attribute_count = attributes.size();
    current_attribute_index = 0;

    ESP_LOGI(TAG, "Set %zu attributes", attribute_count);

    // Update display with first attribute
    update_display_from_attribute();
}

/**
 * @brief Navigate to next attribute
 */
void ui_next_attribute()
{
    if (attributes.empty())
    {
        return;
    }

    current_attribute_index++;
    if (current_attribute_index >= attribute_count)
    {
        current_attribute_index = 0; // Wrap around
    }

    ESP_LOGI(TAG, "Next attribute: index %d", current_attribute_index);
    update_display_from_attribute();
}

/**
 * @brief Navigate to previous attribute
 */
void ui_prev_attribute()
{
    if (attributes.empty())
    {
        return;
    }

    current_attribute_index--;
    if (current_attribute_index < 0)
    {
        current_attribute_index = attribute_count - 1; // Wrap around
    }

    ESP_LOGI(TAG, "Previous attribute: index %d", current_attribute_index);
    update_display_from_attribute();
}

/**
 * @brief Update the title text
 */
void ui_set_title(const char *title)
{
    if (title_label != nullptr)
    {
        lv_label_set_text(title_label, title);
    }
}

/**
 * @brief Update the arc value
 */
void ui_set_value(uint8_t value)
{
    if (arc != nullptr)
    {
        // Get current attribute for type checking
        attribute_t *attr = nullptr;
        if (!attributes.empty())
        {
            attr = &attributes[current_attribute_index];
        }

        // Clamp value to valid range based on attribute type
        if (attr != nullptr)
        {
            if (attr->type == ATTR_TYPE_PROGRAM_CHANGE)
            {
                if (value >= attr->pc.num_programs)
                {
                    value = attr->pc.num_programs - 1;
                }
            }
            else
            {
                if (value > 127)
                {
                    value = 127;
                }
            }

            // Update the attribute's value
            attr->value = value;
        }

        lv_arc_set_value(arc, value);

        // Update center label
        auto *value_label = static_cast<lv_obj_t *>(lv_obj_get_user_data(arc));
        if (value_label != nullptr)
        {
            if (attr != nullptr && attr->type == ATTR_TYPE_PROGRAM_CHANGE)
            {
                // Display program name for Program Change
                if (value < attr->pc.num_programs && attr->pc.program_names[value] != nullptr)
                {
                    lv_label_set_text(value_label, attr->pc.program_names[value]);
                }
                else
                {
                    // Fallback to numeric display
                    auto value_str = std::format("{}", value);
                    lv_label_set_text(value_label, value_str.c_str());
                }
            }
            else
            {
                // Display numeric value for CC
                auto value_str = std::format("{}", value);
                lv_label_set_text(value_label, value_str.c_str());
            }
        }
    }
}

/**
 * @brief Get the current attribute index
 */
int ui_get_current_attribute_index()
{
    return current_attribute_index;
}

/**
 * @brief Get pointer to current attribute
 */
attribute_t *ui_get_current_attribute()
{
    if (attributes.empty())
    {
        return nullptr;
    }
    return &attributes[current_attribute_index];
}

/**
 * @brief LVGL task for periodic display updates
 */
void ui_task(void *pvParameters)
{
    ESP_LOGI(TAG, "UI task started");

    while (1)
    {
        // Run LVGL timer handler
        lv_timer_handler();

        // Delay for refresh period (33ms = ~30 FPS)
        vTaskDelay(pdMS_TO_TICKS(33));
    }
}
