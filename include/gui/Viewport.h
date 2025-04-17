#pragma once

#include "imgui.h"
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

class Viewport{
    float width, height;
    float x, y;
public:
    void ShowViewport();

    glm::vec2 SendCursorPos() const {
        return {x,y};
    };

    glm::vec2 SendSize() const {
        return {width,height};
    };
};