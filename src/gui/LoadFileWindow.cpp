#include "imgui.h"
#include "gui/LoadFileWindow.h"
#include <glm/common.hpp>
#include <serial/project.h>


void LoadFileWindow::populateFileList(const std::string &directory) {
    fileList.clear();
    for (const auto &entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            fileList.push_back(entry.path());
        }
    }
}

void LoadFileWindow::showLoadFileWindow(GameEngine *engine, bool &showLoadFile) {
    ImGui::Begin("Load File Window", NULL);

    populateFileList("../user");

    ImGui::BeginChild("FileList", ImVec2(0,200), true);
    for (const auto &file : fileList) {
        if (ImGui::Selectable(file.stem().string().c_str())) {
            filePath = file;
        }
    }
    ImGui::EndChild();

    ImGui::BeginChild("SelectedFile", ImVec2(0, 50), true);
    ImGui::Text("Selected File: %s", std::filesystem::path(filePath).stem().string().c_str());
    ImGui::EndChild();

    if (ImGui::Button("Load File")) {
        if (!filePath.empty()) {
            DeserializeProject(engine, filePath);
        }
        showLoadFile = false;
    }
    

    ImGui::End();
}