#pragma once
#include "component.h"
#include "scripting/scripting_engine.h"
#include <angelscript.h>
#include <cassert>
#include <memory>
#include <string>



class Script : public Component
{
public:
    const std::string& typeName;
    asIScriptObject* object;
    bool hasRun = false;

    static std::shared_ptr<Script> create(const std::string& name, asIScriptEngine* engine, ScriptingEngine* scriptEngine, GameObject* container)
    {
        return std::make_shared<Script>(name, engine, scriptEngine, container);
    }


    Script(const std::string& name, asIScriptEngine* engine, ScriptingEngine* scriptEngine, GameObject* container): typeName(name), object(nullptr), scriptEngine(scriptEngine), container(container), engine(engine)
    {}

    ~Script()
    {
        release();
    }

    void start()
    {
        asIScriptModule* module = engine->GetModule("ScriptModule"); assert(module != nullptr);
        object = static_cast<asIScriptObject *>(engine->CreateScriptObject(module->GetTypeInfoByName(typeName.c_str())));
        assert(object != nullptr);

        scriptEngine->scriptObjects.insert({this, container});
    }

    void release()
    {
        if (object != nullptr)
        {
            object->Release();
            object = nullptr;
        }

    }
private:
    asIScriptEngine* engine;
    ScriptingEngine* scriptEngine;
    GameObject* container;
};
