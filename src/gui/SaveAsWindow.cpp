#include "imgui.h"
#include "gui/SaveAsWindow.h"
#include <glm/common.hpp>
#include <serial/project.h>
#include <fstream>

void SaveAsWindow::showSaveAsWindow(GameEngine *engine, bool &showSaveAs) {
    ImGui::Begin("Save As Window", NULL);

    ImGui::Text("Project Name");
    ImGui::SameLine();
    char projectNameBuffer[128];
    strncpy(projectNameBuffer, projectName.c_str(), sizeof(projectNameBuffer));
    projectNameBuffer[127] = '\0';
    if(ImGui::InputText("##ProjectName", projectNameBuffer, sizeof(projectNameBuffer))) {
        projectName = projectNameBuffer;
    }

    if (ImGui::Button("Save As")) {
        if (!projectName.empty()) {
            engine->setName(projectName);
            YAML::Node project = SerializeProject(engine);
            std::string filename = "../user/" + engine->getName() + ".yaml";
            std::ofstream yamlFile(filename);
            yamlFile << project;
            yamlFile.close();
        }
        showSaveAs = false;
    }
    ImGui::End();
}