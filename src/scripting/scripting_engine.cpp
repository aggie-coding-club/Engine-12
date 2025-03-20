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

    // Register the function that we want the scripts to call
    r = engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL); assert( r >= 0 );

    // Register the script interface
    r = engine->RegisterInterface("Behavior"); assert( r >= 0 );
    r = engine->RegisterInterfaceMethod("Behavior", "void start()"); assert( r >= 0 );
    r = engine->RegisterInterfaceMethod("Behavior", "void update()"); assert( r >= 0 );

    loadScripts();
    runScripts();
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


