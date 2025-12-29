#include "ui_components.h"
#include "esp_log.h"
#include <algorithm>

static const char *TAG = "UI_Components";

// ============================================================================
// ValueDisplay Implementation
// ============================================================================

ValueDisplay::ValueDisplay(lv_obj_t *parent)
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

    // Create value label (centered) with large font
    valueLabel_ = lv_label_create(container_);
    lv_label_set_text(valueLabel_, "0");
    lv_obj_set_style_text_font(valueLabel_, &lv_font_montserrat_48, 0);
    lv_obj_center(valueLabel_);
}

ValueDisplay::~ValueDisplay()
{
    if (container_)
    {
        lv_obj_delete(container_);
    }
}

void ValueDisplay::setValue(uint8_t value)
{
    lv_arc_set_value(arc_, value);
}

void ValueDisplay::setValueText(const std::string &text)
{
    lv_label_set_text(valueLabel_, text.c_str());
    lv_obj_center(valueLabel_);
}

void ValueDisplay::setRange(uint8_t min, uint8_t max)
{
    lv_arc_set_range(arc_, min, max);
}

// ============================================================================
// ParameterNameDisplay Implementation
// ============================================================================

ParameterNameDisplay::ParameterNameDisplay(lv_obj_t *parent)
{
    // Create container - positioned absolutely
    container_ = lv_obj_create(parent);
    lv_obj_set_size(container_, LV_PCT(80), LV_SIZE_CONTENT);
    lv_obj_clear_flag(container_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(container_, 0, 0);
    lv_obj_set_style_bg_opa(container_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(container_, 5, 0);

    // Position at top-center (above the value)
    lv_obj_align(container_, LV_ALIGN_CENTER, 0, -60);

    // Create name label with large font
    nameLabel_ = lv_label_create(container_);
    lv_label_set_text(nameLabel_, "Select Parameter");
    lv_obj_set_style_text_align(nameLabel_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(nameLabel_, &lv_font_montserrat_24, 0);
    lv_obj_center(nameLabel_);
}

ParameterNameDisplay::~ParameterNameDisplay()
{
    if (container_)
    {
        lv_obj_delete(container_);
    }
}

void ParameterNameDisplay::setParameterName(const std::string &name)
{
    lv_label_set_text(nameLabel_, name.c_str());
    lv_obj_center(nameLabel_);
}

// ============================================================================
// ParameterSelector Implementation
// ============================================================================

ParameterSelector::ParameterSelector(lv_obj_t *parent)
    : selectedIndex_(0), selectCallback_(nullptr)
{

    // Create container with flex layout - positioned at bottom
    container_ = lv_obj_create(parent);
    lv_obj_set_size(container_, LV_PCT(90), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(container_, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(container_, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(container_, 3, 0);
    lv_obj_set_style_pad_gap(container_, 3, 0);
    lv_obj_set_style_border_width(container_, 0, 0);
    lv_obj_set_style_bg_opa(container_, LV_OPA_TRANSP, 0);

    // Position at bottom-center
    lv_obj_align(container_, LV_ALIGN_BOTTOM_MID, 0, -10);
}

ParameterSelector::~ParameterSelector()
{
    if (container_)
    {
        lv_obj_delete(container_);
    }
}

void ParameterSelector::setParameters(const std::vector<std::string> &paramNames)
{
    // Clear existing buttons
    for (auto btn : buttons_)
    {
        lv_obj_delete(btn);
    }
    buttons_.clear();

    // Create new buttons
    for (size_t i = 0; i < paramNames.size(); i++)
    {
        lv_obj_t *btn = lv_button_create(container_);
        lv_obj_set_size(btn, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

        // Initialize all buttons with grey color
        lv_obj_set_style_bg_color(btn, lv_palette_main(LV_PALETTE_GREY), 0);

        lv_obj_t *label = lv_label_create(btn);
        lv_label_set_text(label, paramNames[i].c_str());
        lv_obj_center(label);

        // Store index as user data
        lv_obj_set_user_data(btn, (void *)i);
        lv_obj_add_event_cb(btn, buttonEventHandler, LV_EVENT_CLICKED, this);

        buttons_.push_back(btn);
    }

    if (!buttons_.empty())
    {
        setSelectedIndex(0);
    }
}

void ParameterSelector::setSelectedIndex(size_t index)
{
    if (index >= buttons_.size())
        return;

    // Remove highlight from previously selected button
    if (selectedIndex_ < buttons_.size())
    {
        lv_obj_set_style_bg_color(buttons_[selectedIndex_],
                                  lv_palette_main(LV_PALETTE_GREY), 0);
    }

    // Highlight new selection
    selectedIndex_ = index;
    lv_obj_set_style_bg_color(buttons_[selectedIndex_],
                              lv_palette_main(LV_PALETTE_BLUE), 0);
}

void ParameterSelector::setSelectCallback(SelectCallback callback)
{
    selectCallback_ = callback;
}

void ParameterSelector::buttonEventHandler(lv_event_t *e)
{
    lv_obj_t *btn = (lv_obj_t *)lv_event_get_target(e);
    ParameterSelector *selector = (ParameterSelector *)lv_event_get_user_data(e);

    size_t index = (size_t)lv_obj_get_user_data(btn);

    selector->setSelectedIndex(index);

    if (selector->selectCallback_)
    {
        selector->selectCallback_(index);
    }
}

// ============================================================================
// PageView Implementation
// ============================================================================

PageView::PageView(lv_obj_t *parent, std::shared_ptr<Page> page)
    : page_(page)
{

    // Create main container - full screen, no flex
    container_ = lv_obj_create(parent);
    lv_obj_set_size(container_, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(container_, 0, 0);
    lv_obj_set_style_border_width(container_, 0, 0);

    // Create components in layered order (arc as background)
    valueDisplay_ = new ValueDisplay(container_);
    nameDisplay_ = new ParameterNameDisplay(container_);
    paramSelector_ = new ParameterSelector(container_);

    // Set up parameter selector
    std::vector<std::string> paramNames;
    for (size_t i = 0; i < page_->getParameterCount(); i++)
    {
        auto param = page_->getParameter(i);
        if (param)
        {
            paramNames.push_back(param->getName());
        }
    }
    paramSelector_->setParameters(paramNames);

    // Set callback for parameter selection
    paramSelector_->setSelectCallback([this](size_t index)
                                      { onParameterSelected(index); });

    // Initial update
    updateDisplay();
}

PageView::~PageView()
{
    delete valueDisplay_;
    delete nameDisplay_;
    delete paramSelector_;

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
    auto param = page_->getSelectedParameter();
    if (!param)
        return;

    // Update arc range based on parameter type
    valueDisplay_->setRange(0, param->getMaxValue());
    valueDisplay_->setValue(param->getValue());
    valueDisplay_->setValueText(param->getDisplayValue());
    nameDisplay_->setParameterName(param->getName());
    paramSelector_->setSelectedIndex(page_->getSelectedIndex());
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

void PageView::onParameterSelected(size_t index)
{
    page_->setSelectedIndex(index);
    updateDisplay();

    auto param = page_->getSelectedParameter();
    if (param)
    {
        ESP_LOGI(TAG, "Parameter selected: %s", param->getName().c_str());
    }
}
