#pragma once
#include <stddef.h>

enum COMPONENT_TYPE {
    TRANSFORM,
    MATERIAL,
    MODEL,
    LIGHT,
    RIGID_BODY,
    COLLIDER,
    NUM_COMPONENTS
};

class Component {
private:

public:
    COMPONENT_TYPE type;
    virtual ~Component() = default;
    Component(): type(NUM_COMPONENTS) {}

    static constexpr size_t GetEnumSize() { return NUM_COMPONENTS; };
};