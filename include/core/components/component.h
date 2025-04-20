#pragma once
#include <stddef.h>
#include <memory>

enum COMPONENT_TYPE {
    TRANSFORM,
    MATERIAL,
    MODEL,
    LIGHT,
    RIGID_BODY,
    COLLIDER,
    SCRIPT,
    NUM_ENUM
};

class Component {
private:

public:
    COMPONENT_TYPE type;
    virtual ~Component() = default;
    Component(): type(NUM_COMPONENTS) {}

    static constexpr size_t GetEnumSize() { return NUM_ENUM; };

    virtual std::shared_ptr<Component> Clone() const = 0;
};