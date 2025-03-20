#pragma once

#include "imgui.h"
#include "core/game_engine.h"
#include <glm/common.hpp>
#include <filesystem>

class SaveAsWindow{
    private:
        std::string projectName;
    public:
        void showSaveAsWindow(GameEngine *engine, bool &showSaveAs);
};