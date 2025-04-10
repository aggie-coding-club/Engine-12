#pragma once
#include <vector>

#include "component.h"
#include "script.h"

class ScriptContainer : public Component
{
public:
    std::vector<std::shared_ptr<Script>> scripts = {};
};
