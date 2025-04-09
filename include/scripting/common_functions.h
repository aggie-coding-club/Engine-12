#pragma once
#include <angelscript.h>

#include "simdjson.h"
#include "core/game_engine.h"
#include "core/game_object.h"
#include "core/components/transform.h"

static std::shared_ptr<GameObject> gameObject;

static void message_callback(const asSMessageInfo *msg, void *param)
{
    const char *type = "ERR ";
    if( msg->type == asMSGTYPE_WARNING )
        type = "WARN";
    else if( msg->type == asMSGTYPE_INFORMATION )
        type = "INFO";
    printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}


// Edit this function to log output in the engine
static void print(const std::string &in)
{
    printf(in.c_str());
}

static Scene* getCurrentScene()
{
    return GameEngine::GetCurrScene().get();
}

static GameObject* find(const std::string& name)
{
    std::vector<std::shared_ptr<GameObject>> objects = getCurrentScene()->GetModels();
    for (auto gameObject: objects) {
        if(gameObject->name == name)
        {
            return gameObject.get();
        }
    };

    return nullptr;
}

static void destroy(GameObject& gameObject)
{
    std::vector<std::shared_ptr<GameObject>> &objects = GameEngine::GetCurrScene()->GetModels();

    for(int i = 0; i < objects.size(); i++)
    {
        if(*objects.at(i) == gameObject)
        {
            objects.at(i).reset();
            objects.erase(objects.begin() + i);
        }
    }
}

static void instantiate(GameObject& gameObject)
{

}

