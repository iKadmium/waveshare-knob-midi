#ifndef UI_COMPONENTS_H
#define UI_COMPONENTS_H

#include "lvgl.h"
#include "midi_model.h"
#include <memory>
#include <functional>

/**
 * @brief Value Display Component - Shows large arc with value label
 */
class ValueDisplay
{
public:
    ValueDisplay(lv_obj_t *parent);
    ~ValueDisplay();

    void setValue(uint8_t value);
    void setValueText(const std::string &text);
    void setRange(uint8_t min, uint8_t max);

    lv_obj_t *getContainer() { return container_; }

private:
    lv_obj_t *container_;
    lv_obj_t *arc_;
    lv_obj_t *valueLabel_;
};

/**
 * @brief Parameter Name Display - Shows the currently selected parameter name
 */
class ParameterNameDisplay
{
public:
    ParameterNameDisplay(lv_obj_t *parent);
    ~ParameterNameDisplay();

    void setParameterName(const std::string &name);

    lv_obj_t *getContainer() { return container_; }

private:
    lv_obj_t *container_;
    lv_obj_t *nameLabel_;
};

/**
 * @brief Parameter Selector - Shows buttons for each parameter
 */
class ParameterSelector
{
public:
    using SelectCallback = std::function<void(size_t index)>;

    ParameterSelector(lv_obj_t *parent);
    ~ParameterSelector();

    void setParameters(const std::vector<std::string> &paramNames);
    void setSelectedIndex(size_t index);
    void setSelectCallback(SelectCallback callback);

    lv_obj_t *getContainer() { return container_; }

private:
    static void buttonEventHandler(lv_event_t *e);

    lv_obj_t *container_;
    std::vector<lv_obj_t *> buttons_;
    size_t selectedIndex_;
    SelectCallback selectCallback_;
};

/**
 * @brief Page View - Main UI component combining all elements
 */
class PageView
{
public:
    PageView(lv_obj_t *parent, std::shared_ptr<Page> page);
    ~PageView();

    void update();
    void incrementValue(int8_t delta);
    void selectNextParameter();
    void selectPreviousParameter();

    std::shared_ptr<Page> getPage() { return page_; }
    lv_obj_t *getContainer() { return container_; }

private:
    void updateDisplay();
    void onParameterSelected(size_t index);

    lv_obj_t *container_;
    std::shared_ptr<Page> page_;

    ValueDisplay *valueDisplay_;
    ParameterNameDisplay *nameDisplay_;
    ParameterSelector *paramSelector_;
};

#endif // UI_COMPONENTS_H
