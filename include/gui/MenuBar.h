#pragma once

#include "imgui.h"
#include "core/simulation_manager.h"

class MenuBar{
  public:
    void ShowMenuBar(bool &ShowDetail, bool &ShowView, bool &ShowHierarchy,  bool &ShowLoadFile, bool &ShowSaveAs);
  private:
    SimulationManager *simulationManager;
};