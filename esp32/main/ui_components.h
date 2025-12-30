#ifndef UI_COMPONENTS_H
#define UI_COMPONENTS_H

#include "lvgl.h"
#include "midi_model.h"
#include <memory>
#include <functional>

/**
 * @brief UI Mode enumeration
 */
enum class UIMode
{
    NAVIGATION, // Navigate through parameters with encoder
    CONTROL     // Adjust parameter value with encoder
};

/**
 * @brief Value Display Component - Shows list of parameters with current one centered
 */
class ValueDisplay
{
public:
    ValueDisplay(lv_obj_t *parent);
    ~ValueDisplay();

    void updateParameterList(const std::vector<std::string> &names,
                             const std::vector<std::string> &values,
                             size_t selectedIndex,
                             uint8_t currentValue,
                             uint8_t maxValue,
                             UIMode mode);

    lv_obj_t *getContainer() { return container_; }

private:
    lv_obj_t *container_;
    lv_obj_t *arc_;           // Arc widget showing value
    lv_obj_t *nameLabel_;     // Current parameter name (large, centered)
    lv_obj_t *valueLabel_;    // Current parameter value (large, centered)
    lv_obj_t *prev1Label_;    // -1 parameter name
    lv_obj_t *prev2Label_;    // -2 parameter name
    lv_obj_t *prev3Label_;    // -3 parameter name
    lv_obj_t *next1Label_;    // +1 parameter name
    lv_obj_t *next2Label_;    // +2 parameter name
    lv_obj_t *next3Label_;    // +3 parameter name
    lv_obj_t *modeIndicator_; // Shows current mode (NAV or CTRL)
};

/**
 * @brief Page View - Main UI component managing modes and display
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

    // Mode management
    UIMode getMode() const { return mode_; }
    void toggleMode();
    void handleEncoderRotation(int8_t delta);

    std::shared_ptr<Page> getPage() { return page_; }
    lv_obj_t *getContainer() { return container_; }

private:
    void updateDisplay();
    void setupTouchCallback();
    static void touchEventHandler(lv_event_t *e);

    lv_obj_t *container_;
    std::shared_ptr<Page> page_;
    UIMode mode_;

    ValueDisplay *valueDisplay_;
};

#endif // UI_COMPONENTS_H
