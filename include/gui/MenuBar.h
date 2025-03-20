#pragma once

#include "imgui.h"
#include "core/simulation_manager.h"

class MenuBar{
  public:
    void ShowMenuBar(bool &ShowDetail, bool &ShowView, bool &ShowHierarchy, bool &ShowCameraDebug);
  private:
    SimulationManager *simulationManager;
};
