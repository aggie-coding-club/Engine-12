#include "scripting/scripting_engine.h"
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

// Function to collect all valid scripts into scriptPaths vector
void ScriptingEngine::FindScripts(const std::string& folderPath) {
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

void ScriptingEngine::init() {
    engine = asCreateScriptEngine();

    // Collect valid Scripts
    FindScripts("Assets/");

    // Set the message callback to receive information on errors in human readable form.
    int r = engine->SetMessageCallback(asFUNCTION(message_callback), 0, asCALL_CDECL); assert( r >= 0 );

    RegisterStdString(engine);
    RegisterScriptArray(engine, true);

    registerClasses();

    loadScripts();
    runScripts();
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

    // Register reference objects
    //Transform
    r = engine->RegisterObjectType("Transform", 0, asOBJ_REF | asOBJ_NOCOUNT); assert( r >= 0 );
    r = engine->RegisterObjectProperty("Transform", "vec3 position", asOFFSET(Transform, position)); assert( r >= 0 );
    r = engine->RegisterObjectProperty("Transform", "vec3 rotation", asOFFSET(Transform, rotation)); assert( r >= 0 );
    r = engine->RegisterObjectProperty("Transform", "vec3 scale", asOFFSET(Transform, scale)); assert( r >= 0 );

    // GameObject
    r = engine->RegisterObjectType("GameObject", 0, asOBJ_REF | asOBJ_NOCOUNT); assert( r >= 0 );
    r = engine->RegisterObjectProperty("GameObject", "string name", asOFFSET(GameObject, name)); assert( r >= 0 );
    r = engine->RegisterObjectMethod("GameObject", "Transform@ get_transform() property", asMETHOD(GameObject, get_transform), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("GameObject", "bool hasTag(const string &in)", asMETHOD(GameObject, hasTag), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("GameObject", "bool addTag(const string &in)", asMETHOD(GameObject, addTag), asCALL_THISCALL); assert( r >= 0 );


    // Register the function that we want the scripts to call
    r = engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GameObject@ find(const string &in)", asFUNCTION(find), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("void destroy(GameObject& gameObject)", asFUNCTION(destroy), asCALL_CDECL); assert( r >= 0 );

    // Register the script interface
    r = engine->RegisterInterface("Behavior"); assert( r >= 0 );
    r = engine->RegisterInterfaceMethod("Behavior", "void start()"); assert( r >= 0 );
    r = engine->RegisterInterfaceMethod("Behavior", "void update()"); assert( r >= 0 );
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

    asIScriptModule *mod = engine->GetModule("ScriptModule"); assert(mod != nullptr);
    for(int i = 0; i < mod->GetObjectTypeCount(); i++) {
        asITypeInfo* type = mod->GetObjectTypeByIndex(i);
        if(type->Implements(engine->GetTypeInfoByDecl("Behavior"))) {
            scriptObjects.insert({type->GetName(), (asIScriptObject*)engine->CreateScriptObject(type)});
        }
    }
}


void ScriptingEngine::runScripts() {
    // Find the function that is to be called.
    asIScriptModule *mod = engine->GetModule("ScriptModule"); assert(mod != nullptr);

    for (auto objectPair : scriptObjects) {
        asITypeInfo* type = mod->GetTypeInfoByName(objectPair.first.c_str()); assert(type != nullptr);
        asIScriptObject* object = objectPair.second;

        asIScriptFunction *func = type->GetMethodByDecl("void start()");
        if( func == 0 )
        {
            // The function couldn't be found. Instruct the script writer
            // to include the expected function in the script.
            printf("The script must have the function 'void start()'. Please add it and try again.\n");
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
}

void ScriptingEngine::runScriptUpdate() {
    asIScriptModule *mod = engine->GetModule("ScriptModule"); assert(mod != nullptr);

    for (auto objectPair : scriptObjects) {
        asITypeInfo* type = mod->GetTypeInfoByName(objectPair.first.c_str()); assert(type != nullptr);
        asIScriptObject* object = objectPair.second;

        asIScriptFunction *func = type->GetMethodByDecl("void update()");
        if( func == 0 )
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
}


void ScriptingEngine::cleanUp()
{
    for (auto objectPair : scriptObjects) {
        objectPair.second->Release();
    }

    ctx->Release();
    engine->ShutDownAndRelease();
}


