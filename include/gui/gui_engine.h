#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include "core/game_engine.h"
#include "core/camera.h"

// Window classes
#include "AddObjectWindow.h"
#include "Details.h"
#include "Viewport.h"
#include "FileHierarchy.h"
#include "MenuBar.h"
#include "secondary_menu_bar.h"

class GuiEngine
{
private:
    ImGuiIO* io;
    ImVec4 clearColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    GLFWwindow* window;
    GameEngine* gameEngine;
    ImFont* inter_24;
    ImFont* icons;
    bool showDetail = true;
    bool showHierarchy = true;
    bool showCameraWindow = false;
    bool showAddObject = false;
    bool showLoadFile = false;
    bool showSaveAs = false;

    char objectLocation[128];
    char objectName[128];

    // Windows
    Viewport viewport;
    FileHierarchy fileHierarchy;
    Details details;
    MenuBar menuBar;
    AddObjectWindow addObjectWindow;
    SecondMenuBar secondMenuBar;

public:
    bool showView = true;
    GuiEngine() = default;
    ~GuiEngine() = default;
    bool init(GLFWwindow *window , GameEngine *_game_engine);
    void run();
    glm::vec4 SendViewportInfo() {
        return {viewport.SendCursorPos(),viewport.SendSize()};
    }
    void cleanup();
};
