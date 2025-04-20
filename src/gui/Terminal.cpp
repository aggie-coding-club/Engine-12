// Code for Terminal window

#include "gui/Terminal.h"

static std::vector<std::string> terminalMessages;  // Store Message in Vector

// Function to add Message to Terminal
void Terminal::addTerminalMessage(const std::string& message) 
{
    terminalMessages.push_back(message);
}

void Terminal::ShowTerminal(const std::shared_ptr<Scene>& scene) 
{  
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 DisplaySize = io.DisplaySize;
    
    float TerminalWidth = DisplaySize.x * 0.75f; 
    float TerminalHeight = DisplaySize.y / 3.f - 29 - 31;

    float rightPanelWidth = DisplaySize.x * 0.25f; 

    float bottomPanelHeight = (DisplaySize.y - TerminalHeight); // Remaining height
    float boxWidth = TerminalWidth / 3.0f;  // 1/3 of Terminal Width

    ImVec2 terminalSize(boxWidth, bottomPanelHeight); 
    ImVec2 terminalPos(ImVec2(0, DisplaySize.y/1.5 + 29 + 31));

    // Set Windows Width
    ImGui::SetNextWindowSize(terminalSize);
    // Set Windows Position
    ImGui::SetNextWindowPos(terminalPos, ImGuiCond_Always);


    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    
    ImGui::Begin("Terminal", &this->showTerminal, window_flags);

    ImGui::BeginTabBar("Terminal", ImGuiTabBarFlags_Reorderable);

    if (ImGui::BeginTabItem("Terminal")) {
        // button to clear logs
        if (ImGui::Button("Clear Terminal")) {
            terminalMessages.clear();
        }
        ImGui::Separator();

        // scrollable log display
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, terminalSize.y - 50), true);
        for (const auto& msg : terminalMessages) {
            ImGui::TextUnformatted(msg.c_str());
        }
        ImGui::EndChild();
        ImGui::EndTabItem();
    }
    ImGui::EndTabBar();

    ImGui::End();
}


