// Code for menu bar

#include <imgui_internal.h>
#include "Fonts/Icons/icons_font_awesome_6.h"

#include "gui/secondary_menu_bar.h"

#include <iostream>
#include <ostream>

void SecondMenuBar::ShowSecondaryMenuBar(){

    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.0f, 0.0f));
    ImGuiViewport *v = (&*GImGui)->Viewports[0];
    ImGui::SetNextWindowSize(ImVec2(v->Size.x, 31.f));

    ImGui::SetNextWindowPos(ImVec2(0, 30));

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
    ImGui::Begin("##Second_Menu", nullptr, window_flags);
    ImGui::BeginMenuBar();

    if(ImGui::MenuItem(ICON_FA_FLOPPY_DISK, nullptr)) {
        std::cout << ICON_FA_FLOPPY_DISK << std::endl;
    }

    if(ImGui::BeginMenu("Mode")) {
        if(ImGui::MenuItem("Solid")) {
            std::cout << "Solid" << std::endl;
        }
        if(ImGui::MenuItem("WireFrame")) {
            std::cout << "WireFrame" << std::endl;
        }
        ImGui::EndMenu();
    }

    if(ImGui::MenuItem(ICON_FA_SQUARE_PLUS, nullptr)) {
        std::cout << ICON_FA_SQUARE_PLUS << std::endl;
    }

    if(ImGui::MenuItem(ICON_FA_PAUSE, nullptr)) {
        std::cout << ICON_FA_PAUSE << std::endl;
    }

    if(ImGui::MenuItem(ICON_FA_PLAY, nullptr)) {
        std::cout << ICON_FA_PLAY << std::endl;
    }

    if(ImGui::MenuItem(ICON_FA_FORWARD_STEP, nullptr)) {
        std::cout << ICON_FA_FORWARD_STEP << std::endl;
    }

    ImGui::EndMenuBar();
    ImGui::End();
    ImGui::PopStyleVar();
}
