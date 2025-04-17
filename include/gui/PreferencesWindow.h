#pragma once

#include "imgui.h"


class PreferencesWindow
{
    bool showPreferences = true;
  public:
    void ShowPreferencesWindow(float &mousePreferences, float &movementPreferences);
};