#include "ui_components.h"
#include "esp_log.h"
#include <algorithm>
#include <cstdlib>

static const char* TAG = "UI_Components";

// ============================================================================
// ValueDisplay Implementation
// ============================================================================

ValueDisplay::ValueDisplay(lv_obj_t* parent)
{
    // Create container - full screen
    container_ = lv_obj_create(parent);
    lv_obj_set_size(container_, LV_PCT(100), LV_PCT(100));
    lv_obj_clear_flag(container_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(container_, 0, 0);
    lv_obj_set_style_bg_opa(container_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(container_, 0, 0);

    // Create arc - fill entire round screen (360x360)
    arc_ = lv_arc_create(container_);
    lv_obj_set_size(arc_, 340, 340); // Slightly smaller than screen for padding
    lv_obj_center(arc_);
    lv_arc_set_range(arc_, 0, 127);
    lv_arc_set_value(arc_, 0);
    lv_arc_set_rotation(arc_, 135);
    lv_arc_set_bg_angles(arc_, 0, 270);
    lv_obj_remove_flag(arc_, LV_OBJ_FLAG_CLICKABLE);

    // Style the arc with wider stroke
    static lv_style_t style_arc_indicator;
    lv_style_init(&style_arc_indicator);
    lv_style_set_arc_color(&style_arc_indicator, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_arc_width(&style_arc_indicator, 15);
    lv_obj_add_style(arc_, &style_arc_indicator, LV_PART_INDICATOR);

    // Style the background arc
    static lv_style_t style_arc_bg;
    lv_style_init(&style_arc_bg);
    lv_style_set_arc_width(&style_arc_bg, 15);
    lv_obj_add_style(arc_, &style_arc_bg, LV_PART_MAIN);

    // Create mode indicator (top center for round display)
    modeIndicator_ = lv_label_create(container_);
    lv_label_set_text(modeIndicator_, "NAV");
    lv_obj_set_style_text_font(modeIndicator_, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(modeIndicator_, lv_palette_main(LV_PALETTE_CYAN), 0);
    lv_obj_align(modeIndicator_, LV_ALIGN_TOP_MID, 0, 10);

    // Create previous parameter labels (above center, closer together)
    prev3Label_ = lv_label_create(container_);
    lv_label_set_text(prev3Label_, "");
    lv_obj_set_style_text_font(prev3Label_, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(prev3Label_, lv_color_hex(0x606060), 0);
    lv_obj_set_style_text_align(prev3Label_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(prev3Label_, LV_ALIGN_CENTER, 0, -90);

    prev2Label_ = lv_label_create(container_);
    lv_label_set_text(prev2Label_, "");
    lv_obj_set_style_text_font(prev2Label_, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(prev2Label_, lv_color_hex(0x808080), 0);
    lv_obj_set_style_text_align(prev2Label_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(prev2Label_, LV_ALIGN_CENTER, 0, -65);

    prev1Label_ = lv_label_create(container_);
    lv_label_set_text(prev1Label_, "");
    lv_obj_set_style_text_font(prev1Label_, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(prev1Label_, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_set_style_text_align(prev1Label_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(prev1Label_, LV_ALIGN_CENTER, 0, -40);

    // Create current parameter name label (centered, large)
    nameLabel_ = lv_label_create(container_);
    lv_label_set_text(nameLabel_, "Parameter");
    lv_obj_set_style_text_font(nameLabel_, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(nameLabel_, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_style_text_align(nameLabel_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(nameLabel_, LV_ALIGN_CENTER, 0, -5);

    // Create value label (centered, very large)
    valueLabel_ = lv_label_create(container_);
    lv_label_set_text(valueLabel_, "0");
    lv_obj_set_style_text_font(valueLabel_, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_align(valueLabel_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(valueLabel_, LV_ALIGN_CENTER, 0, 50);

    // Create next parameter labels (below center, closer together)
    next1Label_ = lv_label_create(container_);
    lv_label_set_text(next1Label_, "");
    lv_obj_set_style_text_font(next1Label_, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(next1Label_, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_set_style_text_align(next1Label_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(next1Label_, LV_ALIGN_CENTER, 0, 30);

    next2Label_ = lv_label_create(container_);
    lv_label_set_text(next2Label_, "");
    lv_obj_set_style_text_font(next2Label_, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(next2Label_, lv_color_hex(0x808080), 0);
    lv_obj_set_style_text_align(next2Label_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(next2Label_, LV_ALIGN_CENTER, 0, 55);

    next3Label_ = lv_label_create(container_);
    lv_label_set_text(next3Label_, "");
    lv_obj_set_style_text_font(next3Label_, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(next3Label_, lv_color_hex(0x606060), 0);
    lv_obj_set_style_text_align(next3Label_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(next3Label_, LV_ALIGN_CENTER, 0, 80);
}

ValueDisplay::~ValueDisplay()
{
    if (container_)
    {
        lv_obj_delete(container_);
    }
}

void ValueDisplay::updateParameterList(const std::vector<std::string>& names,
    const std::vector<std::string>& values,
    size_t selectedIndex,
    uint8_t currentValue,
    uint8_t maxValue,
    UIMode mode)
{
    // Update mode indicator
    lv_label_set_text(modeIndicator_, mode == UIMode::NAVIGATION ? "NAV" : "CTRL");

    // Update arc with the actual numeric value
    lv_arc_set_range(arc_, 0, maxValue);
    lv_arc_set_value(arc_, currentValue);

    // Highlight current parameter name in control mode
    if (mode == UIMode::CONTROL)
    {
        lv_obj_set_style_text_color(nameLabel_, lv_palette_main(LV_PALETTE_GREEN), 0);
        // Show value in control mode
        lv_obj_clear_flag(valueLabel_, LV_OBJ_FLAG_HIDDEN);
        // Hide all prev/next labels in control mode
        lv_obj_add_flag(prev1Label_, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(prev2Label_, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(prev3Label_, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(next1Label_, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(next2Label_, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(next3Label_, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_set_style_text_color(nameLabel_, lv_palette_main(LV_PALETTE_BLUE), 0);
        // Hide value in navigation mode
        lv_obj_add_flag(valueLabel_, LV_OBJ_FLAG_HIDDEN);
        // Show prev/next labels in navigation mode
        lv_obj_clear_flag(prev1Label_, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(prev2Label_, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(prev3Label_, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(next1Label_, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(next2Label_, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(next3Label_, LV_OBJ_FLAG_HIDDEN);
    }

    // Update current parameter
    if (selectedIndex < names.size())
    {
        lv_label_set_text(nameLabel_, names[selectedIndex].c_str());
        lv_obj_align(nameLabel_, LV_ALIGN_CENTER, 0, mode == UIMode::CONTROL ? -15 : -5);

        lv_label_set_text(valueLabel_, values[selectedIndex].c_str());
        lv_obj_align(valueLabel_, LV_ALIGN_CENTER, 0, 50);
    }

    // Update previous parameters (3 before) - only matters in NAV mode but update anyway
    if (selectedIndex >= 1)
    {
        lv_label_set_text(prev1Label_, names[selectedIndex - 1].c_str());
        lv_obj_align(prev1Label_, LV_ALIGN_CENTER, 0, -40);
    }
    else
    {
        lv_label_set_text(prev1Label_, "");
    }

    if (selectedIndex >= 2)
    {
        lv_label_set_text(prev2Label_, names[selectedIndex - 2].c_str());
        lv_obj_align(prev2Label_, LV_ALIGN_CENTER, 0, -65);
    }
    else
    {
        lv_label_set_text(prev2Label_, "");
    }

    if (selectedIndex >= 3)
    {
        lv_label_set_text(prev3Label_, names[selectedIndex - 3].c_str());
        lv_obj_align(prev3Label_, LV_ALIGN_CENTER, 0, -90);
    }
    else
    {
        lv_label_set_text(prev3Label_, "");
    }

    // Update next parameters (3 after)
    if (selectedIndex + 1 < names.size())
    {
        lv_label_set_text(next1Label_, names[selectedIndex + 1].c_str());
        lv_obj_align(next1Label_, LV_ALIGN_CENTER, 0, 30);
    }
    else
    {
        lv_label_set_text(next1Label_, "");
    }

    if (selectedIndex + 2 < names.size())
    {
        lv_label_set_text(next2Label_, names[selectedIndex + 2].c_str());
        lv_obj_align(next2Label_, LV_ALIGN_CENTER, 0, 55);
    }
    else
    {
        lv_label_set_text(next2Label_, "");
    }

    if (selectedIndex + 3 < names.size())
    {
        lv_label_set_text(next3Label_, names[selectedIndex + 3].c_str());
        lv_obj_align(next3Label_, LV_ALIGN_CENTER, 0, 80);
    }
    else
    {
        lv_label_set_text(next3Label_, "");
    }
}

// ============================================================================
// PageView Implementation
// ============================================================================

PageView::PageView(lv_obj_t* parent, std::shared_ptr<Page> page)
    : page_(page), mode_(UIMode::NAVIGATION)
{
    // Create main container - full screen
    container_ = lv_obj_create(parent);
    lv_obj_set_size(container_, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(container_, 0, 0);
    lv_obj_set_style_border_width(container_, 0, 0);
    lv_obj_clear_flag(container_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(container_, LV_OPA_TRANSP, 0);

    // Create value display
    valueDisplay_ = new ValueDisplay(container_);

    // Set up touch callback for mode switching - attach to the ValueDisplay container
    setupTouchCallback();

    // Initial update
    updateDisplay();
}

PageView::~PageView()
{
    delete valueDisplay_;

    if (container_)
    {
        lv_obj_delete(container_);
    }
}

void PageView::update()
{
    updateDisplay();
}

void PageView::updateDisplay()
{
    // Gather parameter names and values
    std::vector<std::string> names;
    std::vector<std::string> values;

    for (size_t i = 0; i < page_->getParameterCount(); i++)
    {
        auto param = page_->getParameter(i);
        if (param)
        {
            names.push_back(param->getName());
            values.push_back(param->getDisplayValue());
        }
    }

    // Get the current parameter's numeric value and max value
    uint8_t currentValue = 0;
    uint8_t maxValue = 127;
    auto currentParam = page_->getSelectedParameter();
    if (currentParam)
    {
        currentValue = currentParam->getValue();
        maxValue = currentParam->getMaxValue();
    }

    valueDisplay_->updateParameterList(names, values, page_->getSelectedIndex(),
        currentValue, maxValue, mode_);
}

void PageView::incrementValue(int8_t delta)
{
    auto param = page_->getSelectedParameter();
    if (!param)
        return;

    int16_t newValue = param->getValue() + delta;
    uint8_t maxValue = param->getMaxValue();

    // Clamp to parameter's range
    if (newValue < 0)
        newValue = 0;
    if (newValue > maxValue)
        newValue = maxValue;

    param->setValue(static_cast<uint8_t>(newValue));

    updateDisplay();

    ESP_LOGI(TAG, "Parameter '%s' value changed to %d",
        param->getName().c_str(), param->getValue());
}

void PageView::selectNextParameter()
{
    page_->selectNext();
    updateDisplay();
}

void PageView::selectPreviousParameter()
{
    page_->selectPrevious();
    updateDisplay();
}

void PageView::toggleMode()
{
    if (mode_ == UIMode::NAVIGATION)
    {
        mode_ = UIMode::CONTROL;
        ESP_LOGI(TAG, "Switched to CONTROL mode");
    }
    else
    {
        mode_ = UIMode::NAVIGATION;
        ESP_LOGI(TAG, "Switched to NAVIGATION mode");
    }
    updateDisplay();
}

void PageView::handleEncoderRotation(int8_t delta)
{
    if (mode_ == UIMode::NAVIGATION)
    {
        // Navigate through parameters
        if (delta > 0)
        {
            selectNextParameter();
        }
        else if (delta < 0)
        {
            selectPreviousParameter();
        }
    }
    else // CONTROL mode
    {
        // Adjust current parameter value
        incrementValue(delta);
    }
}

void PageView::setupTouchCallback()
{
    // Add touch event to ValueDisplay container - use PRESSED event for immediate response
    lv_obj_t* display_container = valueDisplay_->getContainer();
    lv_obj_add_flag(display_container, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(display_container, touchEventHandler, LV_EVENT_PRESSED, this);
    ESP_LOGI(TAG, "Touch callback attached to display container");
}

void PageView::touchEventHandler(lv_event_t* e)
{
    ESP_LOGI(TAG, "Touch event detected - toggling mode");
    PageView* pageView = (PageView*) lv_event_get_user_data(e);
    if (pageView)
    {
        pageView->toggleMode();
    }
}
