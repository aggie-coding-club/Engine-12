#pragma once

#include <iostream>
#include <string>
#include <array>
#include <unordered_set>
#include <memory>
#include <angelscript.h>
#include <vector>

#include "components/component.h"
#include "components/script.h"
#include "components/script_container.h"
#include "components/transform.h"

class GameObject {
public:
    std::string name;
    // Contains all the compenents associated with the game object
    // If the game object does not have that component, the element where
    // that component should be is null.
    std::array<std::shared_ptr<Component>, Component::GetEnumSize()> components{};
    std::unordered_set<std::string> tags;
    std::string model_path;

    GameObject(const std::string model_path = "../resources/models/bunny.obj") : model_path(model_path), id(generateUniqueId()) {}
    const int id;

    Transform* get_transform()
    {
        return dynamic_cast<Transform*>(components[TRANSFORM].get());
    }

    bool hasTag(const std::string& tag)
    {
        return tags.find(tag) != tags.end();
    }

    bool addTag(const std::string& tag)
    {
        return tags.insert(tag).second;
    }

    Component* getComponent(COMPONENT_TYPE type){
        return components[type].get();
    }

    void addScript(const std::string& name, ScriptingEngine* engine)
    {
        std::cout << "Adding script " << name << std::endl;

        if(components[SCRIPT] == nullptr)
        {
            components[SCRIPT] = std::make_shared<ScriptContainer>();
        }

        dynamic_cast<ScriptContainer*>(components[SCRIPT].get())->scripts.push_back(Script::create(name, engine->engine, engine, this));
    }

    bool operator==(const GameObject& other) const
    {
        return id == other.id;
    }
private:
    static int lastId;

    static int generateUniqueId() {
        return lastId++;
    }
};
