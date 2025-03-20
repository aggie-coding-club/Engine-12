// Code for menu bar

#include "gui/MenuBar.h"

void MenuBar::ShowMenuBar(bool &ShowDetail, bool &ShowView, bool &ShowHierarchy, bool &ShowCameraDebug){
    if (ImGui::BeginMainMenuBar())
    {
        // File menu
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New")) {
                // Action for New
            }
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
                // Action for Open
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                // Action for Save
            }
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                // Action for Exit
            }
            ImGui::EndMenu();
        }

        // Edit menu
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
                // Action for Undo
            }
            if (ImGui::MenuItem("Redo", "Ctrl+Y")) {
                // Action for Redo
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "Ctrl+X")) {
                // Action for Cut
            }
            if (ImGui::MenuItem("Copy", "Ctrl+C")) {
                // Action for Copy
            }
            if (ImGui::MenuItem("Paste", "Ctrl+V")) {
                // Action for Paste
            }
            ImGui::EndMenu();
        }

        // View menu
        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Show Viewport", nullptr, &ShowView);

            ImGui::MenuItem("Show Details", nullptr, &ShowDetail);

            ImGui::MenuItem("Show File Hierarchy", nullptr, &ShowHierarchy);

            ImGui::MenuItem("Show Camera Debug", nullptr, &ShowCameraDebug);

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Window")) {
            static bool showWindowbar = true;
            ImGui::MenuItem("Temp", nullptr);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            static bool showHelp = true;
            ImGui::MenuItem("About", nullptr);
            ImGui::MenuItem("FAQ", nullptr);
            ImGui::EndMenu();
        }

        // Start of Simulation, probably move out of menu bar
        if(simulationManager.isRunning()) {
            if (ImGui::Button("Pause")) {
                simulationManager.PauseSimulation();
            }
        }
        else {
            if (ImGui::Button("Start")) {
                simulationManager.StartSimulation();
            }
        }
        
        if(!simulationManager.isStopped()) {
            if (ImGui::Button("Stop")) {
                simulationManager.StopSimulation();
            }
        }
        // End of Simulation Button

        ImGui::EndMainMenuBar();
    }
}