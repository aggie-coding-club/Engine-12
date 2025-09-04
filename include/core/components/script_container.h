#pragma once
#include <vector>

#include "component.h"
#include "script.h"

class ScriptContainer : public Component
{
public:
    std::vector<std::shared_ptr<Script>> scripts = {};

    std::shared_ptr<Component> Clone() const override {
        return std::make_shared<ScriptContainer>(*this);
    }
};
