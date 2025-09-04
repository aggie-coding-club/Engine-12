#pragma once

#include "imgui.h"
#include <vector>
#include <string>
#include <memory>

class Scene;

class Terminal {
private:
    bool showTerminal = true;
    ImVec2 size, pos;
    ImVec2 terminalSize; 

public:
    static void addTerminalMessage(const std::string& message); // addTerminalMessage("text"); for debugging
    void ShowTerminal(const std::shared_ptr<Scene>& scene);
    void SetParams(ImVec2 _size, ImVec2 _pos) {
        size = _size;
        pos = _pos;
    };
};
