#include "gui/gui_engine.h"
#include "core/game_engine.h"
#include "fmt/os.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

bool GuiEngine::init(GLFWwindow *_window, GameEngine *_game_engine)
{
    window = _window;
    gameEngine = _game_engine;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
    // Create window with graphics context
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO(); (void)io;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    inter_24 = io->Fonts->AddFontFromFileTTF("../include/Fonts/Inter-VariableFont_opsz,wght.ttf", 24);
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // Temporarily set the resize grip alpha to 0 to hide it
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_ResizeGrip].w = 0.0f;
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Setting End of buffers to '\0' to prevent errors
    objectLocation[127] = '\0';
    objectName[127] = '\0';

    details.SetParms(ImVec2(WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2), ImVec2(WINDOW_WIDTH - (WINDOW_WIDTH / 4),29 + WINDOW_HEIGHT / 2));
    fileHierarchy.SetParms(ImVec2(WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2),ImVec2(WINDOW_WIDTH * 3 / 4,29));
    
    return true;
}

void GuiEngine::run( int width, int height )
{
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Game Engine
    menuBar.ShowMenuBar(gameEngine, showDetail, showView, showHierarchy, showCameraWindow, showLoadFile, showSaveAs);
    if(showHierarchy) {
#ifndef _USE_SCENE_
        ShowFileHierarchy(gameEngine ,gameEngine->GetGameObjects());
#endif
        fileHierarchy.ShowFileHierarchy(gameEngine->GetCurrScene() , showAddObject);
    }
    if(showView)
    {
        viewport.ShowViewport(ImVec2(width, height));
    }
    if(showDetail)
    {
        details.ShowDetails(gameEngine->GetCurrScene());
    }
    if(showCameraWindow){
        cameraDebugWindow.ShowCameraDebugWindow(gameEngine->GetCurrScene()->GetCurrCamera());
    }
    if(showAddObject) {
        addObjectWindow.showAddObjectWindow(gameEngine, showAddObject);
#ifndef _USE_SCENE_
        details.ShowDetails(gameEngine->selectedGameObj);
#endif
    }
    if(showLoadFile) {
        loadFileWindow.showLoadFileWindow(gameEngine, showLoadFile);
    }
    if(showSaveAs) {
        saveAsWindow.showSaveAsWindow(gameEngine, showSaveAs);
    }

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void GuiEngine::cleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
