#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include <cstdint>
#include <cstddef>

/**
 * @brief Attribute type enumeration
 */
enum class AttributeType : uint8_t
{
    CC,            // Continuous Controller (0-127)
    PROGRAM_CHANGE // Program Change with named programs
};

// Legacy compatibility aliases
using attribute_type_t = AttributeType;
constexpr auto ATTR_TYPE_CC = AttributeType::CC;
constexpr auto ATTR_TYPE_PROGRAM_CHANGE = AttributeType::PROGRAM_CHANGE;

/**
 * @brief MIDI attribute definition
 *
 * Represents either a CC parameter or a Program Change parameter with named programs.
 */
struct attribute_t
{
    const char *title = nullptr;            // Display name for the attribute
    AttributeType type = AttributeType::CC; // Type of attribute (CC or Program Change)
    uint8_t channel = 0;                    // MIDI channel (0-15, representing channels 1-16)

    union
    {
        uint8_t cc_number; // MIDI CC number (0-127) - for CC type
        struct
        {
            const char **program_names; // Array of program names - for Program Change type
            uint8_t num_programs;       // Number of programs in the array
        } pc;
    };

    uint8_t value = 0; // Current value (0-127 for CC, 0-num_programs-1 for PC)

    // Default constructor
    attribute_t() : cc_number(0) {}

    // Copy constructor
    attribute_t(const attribute_t &other)
        : title(other.title), type(other.type), channel(other.channel), value(other.value)
    {
        if (type == AttributeType::PROGRAM_CHANGE)
        {
            pc = other.pc;
        }
        else
        {
            cc_number = other.cc_number;
        }
    }

    // Copy assignment operator
    attribute_t &operator=(const attribute_t &other)
    {
        if (this != &other)
        {
            title = other.title;
            type = other.type;
            channel = other.channel;
            value = other.value;
            if (type == AttributeType::PROGRAM_CHANGE)
            {
                pc = other.pc;
            }
            else
            {
                cc_number = other.cc_number;
            }
        }
        return *this;
    }
};

#endif // ATTRIBUTES_H
