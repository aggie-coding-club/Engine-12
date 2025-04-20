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
#include "components/material.h"
#include "components/model.h"
#include "components/rigidbody.h"
#include "components/light.h"

class GameObject {
public:
    std::string name;
    // Contains all the compenents associated with the game object
    // If the game object does not have that component, the element where
    // that component should be is null.
    std::array<std::shared_ptr<Component>, Component::GetEnumSize()> components{};
    std::unordered_set<std::string> tags;
    std::string model_path;

    GameObject(const std::string model_path = "../resources/models/sphere.obj") : model_path(model_path), id(generateUniqueId()) {}
    const int id;

    Transform* get_transform()
    {
        return dynamic_cast<Transform*>(components[TRANSFORM].get());
    }

    // returns a reference to the component of the given type
    Component* GetComponent(COMPONENT_TYPE type) {
        return components[type].get();
    }

    // returns a reference to the component by the index
    /*Component* GetComponent(size_t index) {
        return components[index].get();
    }*/

    // returns the amount of components
    int GetComponentCount() {
        int count = 0;

        for(std::shared_ptr<Component> component : components) {
            if(component) {
                count++;
            }

        }
        return count;
    }

    // returns true if tag is present in the game object
    bool CompareTag(const std::string& tag) const {
        return tags.find(tag) != tags.end();
    }

    // adds a tag to the game object
    void AddTag(const std::string& tag) {
        tags.insert(tag);
    }

    // removes a tag from the game object
    void RemoveTag(const std::string& tag) {
        tags.erase(tag);
    }

    void AddComponent(COMPONENT_TYPE component) {
        switch(component) {
            // case TRANSFORM:
            //     if(!components[TRANSFORM]) {
            //         components[TRANSFORM] = std::make_shared<Transform>();
            //     }
            //     break;
            case MATERIAL:
                if(!components[MATERIAL]) {
                    components[MATERIAL] = std::make_shared<Material>();
                }
                break;
            case MODEL:
                if(!components[MODEL]) {
                    components[MODEL] = std::make_shared<Model>();
                }
                break;
            case LIGHT:
                if(!components[LIGHT]) {
                    components[LIGHT] = std::make_shared<PointLight>();
                }
                break;
            case RIGID_BODY:
                if(!components[RIGID_BODY]) {
                    components[RIGID_BODY] = std::make_shared<RigidBody>();
                }
                break;
        }
    }

    // Don't call on transform
    void RemoveComponent(COMPONENT_TYPE type) {
        if(components[type] && type != TRANSFORM) {
            components[type] = nullptr;
        }
    }

    void CopyComponent(const std::shared_ptr<Component> component) {
        if(component) {
            COMPONENT_TYPE type = component->type;
            components[type] = component->Clone();
        }
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
