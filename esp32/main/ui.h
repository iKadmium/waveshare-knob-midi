#ifndef UI_H
#define UI_H

#include <cstdint>
#include <vector>
#include "attributes.h"

/**
 * @brief Initialize the LVGL UI with display and touch drivers
 *
 * This function sets up the display driver, creates the UI elements including
 * the title label, arc display, and gesture detection for attribute navigation.
 *
 * @return 0 on success, negative value on error
 */
int ui_init();

/**
 * @brief Set the attributes array for the UI to manage
 *
 * @param attrs Pointer to array of attributes
 * @param count Number of attributes in the array
 */
void ui_set_attributes(std::vector<attribute_t> &attrs);

/**
 * @brief Update the active attribute title
 *
 * @param title The title text to display (will be centered)
 */
void ui_set_title(const char *title);

/**
 * @brief Update the arc value display
 *
 * @param value Value from 0-127 to display on the arc
 */
void ui_set_value(uint8_t value);

/**
 * @brief Get the current attribute index
 *
 * @return The current attribute index
 */
int ui_get_current_attribute_index();

/**
 * @brief Get pointer to the current active attribute
 *
 * @return Pointer to current attribute, or nullptr if no attributes set
 */
attribute_t *ui_get_current_attribute();

/**
 * @brief Navigate to the next attribute
 */
void ui_next_attribute();

/**
 * @brief Navigate to the previous attribute
 */
void ui_prev_attribute();

/**
 * @brief LVGL task that handles display refresh
 *
 * This task should be called periodically to update the display.
 * It runs the LVGL timer handler.
 *
 * @param pvParameters FreeRTOS task parameters (unused)
 */
void ui_task(void *pvParameters);

#endif // UI_H
