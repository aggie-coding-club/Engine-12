#pragma once

#include "imgui.h"
#include "core/game_engine.h"
#include "core/simulation_manager.h"

class MenuBar{
  public:
    void ShowMenuBar(GameEngine *engine, bool &ShowDetail, bool &ShowView, bool &ShowHierarchy,  bool &ShowLoadFile, bool &ShowSaveAs);
  private:
    SimulationManager *simulationManager;
};