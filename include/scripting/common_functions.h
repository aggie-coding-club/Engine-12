#pragma once
#include <angelscript.h>

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

