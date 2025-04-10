#include "scripting/scripting_engine.h"
#include "core/components/script.h"
#include "scripting/common_functions.h"
#include <cassert>
#include <utility>
#include <vector>
#include <scriptarray/scriptarray.h>

#include "core/game_object.h"
#include "core/components/transform.h"
#include <iostream>
#include <filesystem>

#include "scripting/wrappers.h"

ScriptingEngine* ScriptingEngine::instance;

// Function to collect all valid scripts into scriptPaths vector
void ScriptingEngine::FindScripts(const std::string& folderPath) {
    scriptPaths = {};
    namespace fs = std::filesystem;
    try {
        for (const fs::directory_entry& entry : fs::recursive_directory_iterator(folderPath)) {
            if (entry.path().extension() == ".as") { // Only add files with .as extension
                scriptPaths.push_back(entry.path().string());
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
}

void ScriptingEngine::init(SimulationManager* sim) {
    engine = asCreateScriptEngine();
    simulation = sim;
    instance = this;

    // Collect valid Scripts
    FindScripts("Assets/");

    // Set the message callback to receive information on errors in human readable form.
    int r = engine->SetMessageCallback(asFUNCTION(message_callback), 0, asCALL_CDECL); assert( r >= 0 );

    RegisterStdString(engine);
    RegisterScriptArray(engine, true);

    registerClasses();
}

void ScriptingEngine::registerClasses() {
    //Register value objects
    //vec3
    int r = engine->RegisterObjectType("vec3", sizeof(glm::vec3), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS); assert(r >= 0);
    r = engine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ConstructVec3), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f(const vec3 &in)", asFUNCTION(CopyConstructVec3), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f(float, float, float)", asFUNCTION(ConstructVec3Float3), asCALL_CDECL_OBJLAST); assert(r >= 0);

    r = engine->RegisterObjectProperty("vec3", "float x", asOFFSET(glm::vec3, x)); assert(r >= 0);
    r = engine->RegisterObjectProperty("vec3", "float y", asOFFSET(glm::vec3, y)); assert(r >= 0);
    r = engine->RegisterObjectProperty("vec3", "float z", asOFFSET(glm::vec3, z)); assert(r >= 0);

    //Enums
    r = engine->RegisterEnum("COMPONENT_TYPE"); assert(r >= 0);
    r = engine->RegisterEnumValue("COMPONENT_TYPE", "TRANSFORM", TRANSFORM); assert(r >= 0);
    r = engine->RegisterEnumValue("COMPONENT_TYPE", "MATERIAL", MATERIAL); assert(r >= 0);
    r = engine->RegisterEnumValue("COMPONENT_TYPE", "MODEL", MODEL); assert(r >= 0);
    r = engine->RegisterEnumValue("COMPONENT_TYPE", "LIGHT", LIGHT); assert(r >= 0);
    r = engine->RegisterEnumValue("COMPONENT_TYPE", "RIGID_BODY", RIGID_BODY); assert(r >= 0);
    r = engine->RegisterEnumValue("COMPONENT_TYPE", "SCRIPT", SCRIPT); assert(r >= 0);

    // Register reference objects
    //Transform
    r = engine->RegisterObjectType("Transform", 0, asOBJ_REF | asOBJ_NOCOUNT); assert( r >= 0 );
    r = engine->RegisterObjectProperty("Transform", "vec3 position", asOFFSET(Transform, position)); assert( r >= 0 );
    r = engine->RegisterObjectProperty("Transform", "vec3 rotation", asOFFSET(Transform, rotation)); assert( r >= 0 );
    r = engine->RegisterObjectProperty("Transform", "vec3 scale", asOFFSET(Transform, scale)); assert( r >= 0 );

    //Material
    r = engine->RegisterObjectType("Material", 0, asOBJ_REF | asOBJ_NOCOUNT); assert( r >= 0 );
    r = engine->RegisterObjectProperty("Material", "vec3 ambient", asOFFSET(Material, ambient)); assert( r >= 0 );
    r = engine->RegisterObjectProperty("Material", "vec3 diffuse", asOFFSET(Material, diffuse)); assert( r >= 0 );
    r = engine->RegisterObjectProperty("Material", "vec3 specular", asOFFSET(Material, specular)); assert( r >= 0 );
    r = engine->RegisterObjectProperty("Material", "float shininess", asOFFSET(Material, shininess)); assert( r >= 0 );

    //Rigid body
    r = engine->RegisterObjectType("RigidBody", 0, asOBJ_REF | asOBJ_NOCOUNT); assert( r >= 0 );
    r = engine->RegisterObjectProperty("RigidBody", "vec3 centerOfMass", asOFFSET(RigidBody, centerOfMass)); assert( r >= 0 );
    r = engine->RegisterObjectProperty("RigidBody", "vec3 velocity", asOFFSET(RigidBody, velocity)); assert( r >= 0 );
    r = engine->RegisterObjectProperty("RigidBody", "vec3 angularVelocity", asOFFSET(RigidBody, angularVelocity)); assert( r >= 0 );
    r = engine->RegisterObjectProperty("RigidBody", "float maxVelocity", asOFFSET(RigidBody, maxVelocity)); assert( r >= 0 );
    r = engine->RegisterObjectProperty("RigidBody", "vec3 maxAngularVelocity", asOFFSET(RigidBody, maxAngularVelocity)); assert( r >= 0 );
    r = engine->RegisterObjectProperty("RigidBody", "vec3 gravity", asOFFSET(RigidBody, gravity)); assert( r >= 0 );
    r = engine->RegisterObjectProperty("RigidBody", "float mass", asOFFSET(RigidBody, mass)); assert( r >= 0 );
    r = engine->RegisterObjectProperty("RigidBody", "bool autoCenterOfMass", asOFFSET(RigidBody, autoCenterOfMass)); assert( r >= 0 );
    r = engine->RegisterObjectProperty("RigidBody", "bool detectCollisions", asOFFSET(RigidBody, detectCollisions)); assert( r >= 0 );
    r = engine->RegisterObjectProperty("RigidBody", "bool useGravity", asOFFSET(RigidBody, useGravity)); assert( r >= 0 );
    r = engine->RegisterObjectProperty("RigidBody", "bool isKinematics", asOFFSET(RigidBody, isKinematics)); assert( r >= 0 );

    //Component
    r = engine->RegisterObjectType("Component", 0, asOBJ_REF | asOBJ_NOCOUNT); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Component", "Transform@ opCast()", asFUNCTION((refCast<Component,Transform>)), asCALL_CDECL_OBJLAST); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Component", "Material@ opCast()", asFUNCTION((refCast<Component,Material>)), asCALL_CDECL_OBJLAST); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Component", "RigidBody@ opCast()", asFUNCTION((refCast<Component,RigidBody>)), asCALL_CDECL_OBJLAST); assert( r >= 0 );

    // GameObject
    r = engine->RegisterObjectType("GameObject", 0, asOBJ_REF | asOBJ_NOCOUNT); assert( r >= 0 );
    r = engine->RegisterObjectProperty("GameObject", "string name", asOFFSET(GameObject, name)); assert( r >= 0 );
    r = engine->RegisterObjectMethod("GameObject", "Transform@ get_transform() property", asMETHOD(GameObject, get_transform), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("GameObject", "bool hasTag(const string &in)", asMETHOD(GameObject, hasTag), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("GameObject", "bool addTag(const string &in)", asMETHOD(GameObject, addTag), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("GameObject", "Component@ getComponent(COMPONENT_TYPE type)", asMETHOD(GameObject, getComponent), asCALL_THISCALL); assert( r >= 0 );

    // Register the function that we want the scripts to call
    r = engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GameObject@ find(const string &in)", asFUNCTION(find), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("void destroy(GameObject& gameObject)", asFUNCTION(destroy), asCALL_CDECL); assert( r >= 0 );

    // Register the script interface
    r = engine->RegisterInterface("Behavior"); assert( r >= 0 );
    r = engine->RegisterInterfaceMethod("Behavior", "void start()"); assert( r >= 0 );
    r = engine->RegisterInterfaceMethod("Behavior", "void update()"); assert( r >= 0 );
    r = engine->RegisterInterfaceMethod("Behavior", "void stop()"); assert( r >= 0 );
}

void ScriptingEngine::run() {
    // Engine is just started.
    if(!engineRunning && simulation->isRunning())
    {
        engineRunning = true;
        loadScripts();
        runScriptStart();
    }
    // Engine is just stopped.
    else if(engineRunning && simulation->isStopped())
    {
        engineRunning = false;
        stopScripts();
    }
    // Engine is running
    else if(simulation->isRunning())
    {
        runScriptUpdate();
    }
}


void ScriptingEngine::loadScripts() {
    CScriptBuilder builder;
    int r = builder.StartNewModule(engine, "ScriptModule");
    if( r < 0 )
    {
        // If the code fails here it is usually because there
        // is no more memory to allocate the module
        printf("Unrecoverable error while starting a new module.\n");
        return;
    }
    for (int i = 0; i < scriptPaths.size(); i++) {
        r = builder.AddSectionFromFile(scriptPaths.at(i).c_str());
        if( r < 0 )
        {
            // The builder wasn't able to load the file. Maybe the file
            // has been removed, or the wrong name was given, or some
            // preprocessing commands are incorrectly written.
            printf("Please correct the errors in the script and try again.\n");
            return;
        }
    }
    r = builder.BuildModule();
    if( r < 0 ) {
        // An error occurred. Instruct the script writer to fix the
        // compilation errors that were listed in the output stream.
        printf("Please correct the errors in the script and try again.\n");
        return;
    }

    module = engine->GetModule("ScriptModule"); assert(module != nullptr);
    for (const auto& object: getCurrentScene()->GetModels()) {
        if(object->components[SCRIPT] != nullptr)
        {
            for(const auto& script: dynamic_cast<ScriptContainer*>(object->components[SCRIPT].get())->scripts)
            {
                script->start();
            }
        }
    }
}


void ScriptingEngine::runScriptStart() {
    // Find the function that is to be called.
    for (const auto& objectPair : scriptObjects) {
        runFunction(objectPair.first->typeName, objectPair.first->object, "void start()");
    }
}

void ScriptingEngine::runScriptUpdate() {
    for (auto objectPair : scriptObjects) {
        runFunction(objectPair.first->typeName, objectPair.first->object, "void update()");
    }
}

void ScriptingEngine::stopScripts() {
    for (auto objectPair : scriptObjects) {
        runFunction(objectPair.first->typeName, objectPair.first->object, "void stop()");
    }

    for (const auto& objectPair : scriptObjects) {
        objectPair.first->release();
    }
    scriptObjects = {};
    module->Discard();
    module = nullptr;
}

void ScriptingEngine::runFunction(const std::string& typeName, asIScriptObject* object, const std::string& declaration) {
    asITypeInfo* type = module->GetTypeInfoByName(typeName.c_str()); assert(type != nullptr);

    asIScriptFunction *func = type->GetMethodByDecl(declaration.c_str());
    if(func == nullptr)
    {
        // The function couldn't be found. Instruct the script writer
        // to include the expected function in the script.
        printf("The script must have the function 'void update()'. Please add it and try again.\n");
        return;
    }

    // Create our context, prepare it, and then execute
    ctx = engine->CreateContext();
    ctx->Prepare(func);
    ctx->SetObject(object);
    int r = ctx->Execute();
    if(r != asEXECUTION_FINISHED) {
        // The execution didn't complete as expected. Determine what happened.
        if( r == asEXECUTION_EXCEPTION )
        {
            // An exception occurred, let the script writer know what happened so it can be corrected.
            printf("An exception '%s' occurred. Please correct the code and try again.\n", ctx->GetExceptionString());
        }
    }
}

void ScriptingEngine::runFunction(const Script& script, const std::string &declaration) {
    runFunction(script.typeName, script.object, declaration);
}


void ScriptingEngine::cleanUp()
{
    stopScripts();
    ctx->Release();
    engine->ShutDownAndRelease();
}


