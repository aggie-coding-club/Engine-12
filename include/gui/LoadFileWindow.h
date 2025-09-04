#pragma once

#include "imgui.h"
#include "core/game_engine.h"
#include <glm/common.hpp>
#include <filesystem>

class LoadFileWindow{
    private:
        std::string filePath;
        std::vector<std::filesystem::path> fileList;
        void populateFileList(const std::string &directory);
    public:
        void showLoadFileWindow(GameEngine *engine, bool &showLoadFile);
};