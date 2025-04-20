#pragma once
#include <angelscript.h>
#include <unordered_map>
#include <scriptstdstring/scriptstdstring.h>
#include <scriptbuilder/scriptbuilder.h>
#include <vector>

#include "core/simulation_manager.h"

class Script;
class GameObject;

class ScriptingEngine
{
    asIScriptModule* module = nullptr;
    asIScriptContext* ctx = nullptr;
    SimulationManager* simulation = nullptr;
    bool engineRunning = false;
    std::vector<std::string> scriptPaths;

    void FindScripts(const std::string& folderPath);
    void loadScripts();
    void runScriptStart();
    void runScriptUpdate();
    void registerClasses();
    void registerKeys();
    void stopScripts();
    void runFunction(const std::string& typeName, asIScriptObject* object, const std::string& declaration);
    void runFunction(const Script& script, const std::string& declaration);
public:
    static ScriptingEngine* instance;
    std::unordered_map<Script*, GameObject*> scriptObjects;
    asIScriptEngine* engine = nullptr;

    void init(SimulationManager* sim);
    void cleanUp();
    void run();
};
