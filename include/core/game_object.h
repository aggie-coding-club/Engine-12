#pragma once

#include <string>
#include <array>
#include <unordered_set>
#include <memory>

#include "components/component.h"

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

    // returns a reference to the component of the given type
    std::shared_ptr<Component>& GetComponent(COMPONENT_TYPE type) {
        return components[type];
    }

    // returns a reference to the component by the index
    std::shared_ptr<Component>& GetComponent(size_t index) {
        return components[index];
    }

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

private:

    static int lastId;

    static int generateUniqueId() {
        return lastId++;
    }
};
