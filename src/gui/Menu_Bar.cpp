// Code for menu bar

#include <imgui_internal.h>

#include "gui/MenuBar.h"
#include "serial/project.h"
#include <yaml-cpp/yaml.h>
#include <fstream>

void MenuBar::ShowMenuBar(GameEngine *engine, bool &ShowDetail, bool &ShowView, bool &ShowHierarchy, bool &ShowLoadFile, bool &ShowSaveAs) {
    if (ImGui::BeginMainMenuBar()) {
        ImGui::Text(engine->getName().c_str());
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(2.5f, 0.0f));  
        ImGui::SameLine();

        // File menu
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New")) {
                // Action for New
            }
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
                // Action for Open
                ShowLoadFile = true;
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                // Action for Save
                YAML::Node project = SerializeProject(engine);
                std::string filename = "../user/" + engine->getName() + ".yaml";
                std::ofstream fout(filename);
                fout << project;    
                fout.close();
            }
            if (ImGui::MenuItem("Save as", "Ctrl+Shift+S")) {
                // Action for Save
                ShowSaveAs = true;
            }
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                // Action for Exit
            }
            ImGui::EndMenu();
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

    }
    ImGui::EndMainMenuBar();
}