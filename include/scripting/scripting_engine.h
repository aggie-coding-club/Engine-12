#pragma once
#include <angelscript.h>
#include <unordered_map>
#include <scriptstdstring/scriptstdstring.h>
#include <scriptbuilder/scriptbuilder.h>
#include <vector>

#include "core/game_object.h"

class ScriptingEngine
{
    asIScriptEngine* engine = nullptr;
    asIScriptContext* ctx = nullptr;
    std::vector<std::string> scriptPaths;
    std::unordered_map<std::string, asIScriptObject*> scriptObjects;

public:
    void FindScripts(const std::string& folderPath);
    void init();
    void cleanUp();
    void loadScripts();
    void runScripts();
    void runScriptUpdate();
};
