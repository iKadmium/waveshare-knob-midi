#ifndef MIDI_MODEL_H
#define MIDI_MODEL_H

#include <string>
#include <vector>
#include <memory>
#include <stdint.h>

/**
 * @brief Parameter type enumeration
 */
enum class ParameterType
{
    CC,
    PROGRAM_CHANGE
};

/**
 * @brief Base class for MIDI parameters
 */
class Parameter
{
public:
    Parameter(const std::string& name, uint8_t channel)
        : name_(name), channel_(channel & 0x0F), value_(0)
    {
    }

    virtual ~Parameter() = default;

    // Getters
    const std::string& getName() const { return name_; }
    uint8_t getChannel() const { return channel_; }
    uint8_t getValue() const { return value_; }
    virtual ParameterType getType() const = 0;

    // Setter with range validation (0-127 for MIDI)
    void setValue(uint8_t value)
    {
        value_ = value & 0x7F; // Ensure 7-bit value
    }

    // Virtual method for getting display text
    virtual std::string getDisplayValue() const
    {
        return std::to_string(value_);
    }

    // Virtual method for getting max value
    virtual uint8_t getMaxValue() const
    {
        return 127; // Default MIDI range
    }

protected:
    std::string name_;
    uint8_t channel_; // 4-bit (0-15)
    uint8_t value_;   // 7-bit (0-127)
};

/**
 * @brief CC (Control Change) Parameter
 */
class CCParameter : public Parameter
{
public:
    CCParameter(const std::string& name, uint8_t channel, uint8_t ccNumber)
        : Parameter(name, channel), ccNumber_(ccNumber & 0x7F)
    {
    }

    ParameterType getType() const override { return ParameterType::CC; }

    uint8_t getCCNumber() const { return ccNumber_; }

    std::string getDisplayValue() const override
    {
        return std::to_string(value_);
    }

private:
    uint8_t ccNumber_; // 7-bit (0-127)
};

/**
 * @brief Program Change Parameter
 */
class ProgramChangeParameter : public Parameter
{
public:
    ProgramChangeParameter(const std::string& name, uint8_t channel,
        const std::vector<std::string>& programNames)
        : Parameter(name, channel), programNames_(programNames)
    {
    }

    ParameterType getType() const override { return ParameterType::PROGRAM_CHANGE; }

    const std::vector<std::string>& getProgramNames() const { return programNames_; }

    std::string getDisplayValue() const override
    {
        if (value_ < programNames_.size())
        {
            return std::to_string(value_) + ": " + programNames_[value_];
        }
        return "Program " + std::to_string(value_);
    }

    uint8_t getMaxValue() const
    {
        return programNames_.empty() ? 127 : (programNames_.size() - 1);
    }

private:
    std::vector<std::string> programNames_;
};

/**
 * @brief Page containing a collection of parameters
 */
class Page
{
public:
    Page(const std::string& name) : name_(name), selectedIndex_(0) {}

    void addParameter(std::shared_ptr<Parameter> param)
    {
        parameters_.push_back(param);
    }

    const std::string& getName() const { return name_; }

    size_t getParameterCount() const { return parameters_.size(); }

    std::shared_ptr<Parameter> getParameter(size_t index)
    {
        if (index < parameters_.size())
        {
            return parameters_[index];
        }
        return nullptr;
    }

    std::shared_ptr<Parameter> getSelectedParameter()
    {
        return getParameter(selectedIndex_);
    }

    size_t getSelectedIndex() const { return selectedIndex_; }

    void setSelectedIndex(size_t index)
    {
        if (index < parameters_.size())
        {
            selectedIndex_ = index;
        }
    }

    void selectNext()
    {
        if (!parameters_.empty())
        {
            selectedIndex_ = (selectedIndex_ + 1) % parameters_.size();
        }
    }

    void selectPrevious()
    {
        if (!parameters_.empty())
        {
            if (selectedIndex_ == 0)
            {
                selectedIndex_ = parameters_.size() - 1;
            }
            else
            {
                selectedIndex_--;
            }
        }
    }

private:
    std::string name_;
    std::vector<std::shared_ptr<Parameter>> parameters_;
    size_t selectedIndex_;
};

#endif // MIDI_MODEL_H
