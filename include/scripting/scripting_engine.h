#pragma once
#include <angelscript.h>
#include <unordered_map>
#include <scriptstdstring/scriptstdstring.h>
#include <scriptbuilder/scriptbuilder.h>
#include <vector>

#include "core/game_object.h"
#include "core/simulation_manager.h"

class ScriptingEngine
{
    asIScriptEngine* engine = nullptr;
    asIScriptContext* ctx = nullptr;
    SimulationManager* simulation = nullptr;
    std::vector<std::string> scriptPaths;
    std::unordered_map<std::string, asIScriptObject*> scriptObjects;

public:
    void FindScripts(const std::string& folderPath);
    void init(SimulationManager* sim);
    void registerClasses();
    void cleanUp();
    void loadScripts();
    void runScripts();
    void runScriptUpdate();
};
