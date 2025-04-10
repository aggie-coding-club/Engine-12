#pragma once
#include <stddef.h>

enum COMPONENT_TYPE {
    TRANSFORM,
    MATERIAL,
    MODEL,
    LIGHT,
    RIGID_BODY,
    SCRIPT,
    NUM_ENUM
};

class Component {
private:

public:
    COMPONENT_TYPE type;
    virtual ~Component() = default;
    Component(): type(NUM_ENUM) {}

    static constexpr size_t GetEnumSize() { return NUM_ENUM; };
};