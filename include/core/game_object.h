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
private:

    static int lastId;

    static int generateUniqueId() {
        return lastId++;
    }
};
