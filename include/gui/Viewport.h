#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui.h"

class Viewport{
  public:
    void ShowViewport(unsigned int& textureColorbuffer);
};