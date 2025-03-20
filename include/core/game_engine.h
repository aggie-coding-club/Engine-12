#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

#include "game_object.h"
#include "components/material.h"
#include "components/transform.h"
#include "components/light.h"
#include "scene.h"

#define _USE_SCENE_

class GameEngine
{
private:
    std::string name;
    std::vector<std::shared_ptr<Scene>> scenes;
    int currSceneIdx = 0;

    bool changedScene = true;

    void TestInit2()
    {
        const auto scene = std::make_shared<Scene>();

        /**** Camera ****/
        const auto camera = std::make_shared<Camera>();

        scene->GetCameras().emplace_back(camera);
        constexpr size_t n = 3;
        constexpr glm::vec3 pos[n] = {
            {0.0f, -1.0f, 0.0f},
            {-2.0f, -1.0f, -3.0f},
            {2.0f, -1.0f, -3.0f}
        };
        for (int i = 0; i < n; i++)
        {
            scene->AddModel();
            const auto& obj = scene->GetModels().at(i);

            std::dynamic_pointer_cast<Transform>(obj->components[TRANSFORM])->position = pos[i];
        }

	    // // Lights
	    // lights[0].position = {0.0f, 0.0f, 3.0f};
	    // lights[0].color    = {0.5f, 0.5f, 0.5f};

	    // lights[1].position = {0.0f, 3.0f, 0.0f};
	    // lights[1].color    = {0.2f, 0.2f, 0.2f};
        const auto light1 = std::make_shared<GameObject>();
        const auto lightTransform1 = std::make_shared<Transform>(
            glm::vec3(0.0f, 0.0f, 3.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f));

        const auto lightComp1 = std::make_shared<Light>(
            glm::vec3(0.5f, 0.5f, 0.5f),
                1.0f);

        light1->components[TRANSFORM] = lightTransform1;
        light1->components[LIGHT] = lightComp1;

        scene->GetLights().emplace_back(light1);

        const auto light2 = std::make_shared<GameObject>();
        const auto lightTransform2 = std::make_shared<Transform>(
            glm::vec3(0.0f, 3.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f));

        const auto lightComp2 = std::make_shared<Light>(
            glm::vec3(0.2f, 0.2f, 0.2f),
                1.0f);

        light2->components[TRANSFORM] = lightTransform2;
        light2->components[LIGHT] = lightComp2;

        scene->GetLights().emplace_back(light2);

        // Light End


        scenes.push_back(scene);
    }

public:
    std::vector<std::shared_ptr<Scene>>& GetScenes() { return scenes; }
    std::shared_ptr<Scene>& GetCurrScene() { return scenes[currSceneIdx]; }
    GameEngine()
    {
        //TestInit2();
    }

    bool HasChangedScene() const { return changedScene; }
    void ChangedSceneAcknowledged() { changedScene = false; }
    void ChangeScene(const int idx) { currSceneIdx = idx; changedScene = true; }
    void CycleScene() { currSceneIdx = (currSceneIdx+1) % scenes.size(); changedScene = true; }

    std::string& getName() {
        return name;
    }

    void SetScenes(std::vector<std::shared_ptr<Scene>> _scenes) {
        scenes = _scenes;
    }

    void setName(const std::string _name) {
        name = _name;
    }
};

#ifndef _USE_SCENE_
class GameEngine {
private:
    std::vector<std::shared_ptr<GameObject>> gameObjects;
    int randomID = 0;
    std::string modelPath = "../resources/models/";
public:

    std::shared_ptr<GameObject> selectedGameObj = nullptr;

    void AddObjectToVerticesMap(std::shared_ptr<GameObject> gameObject);

    void TestInit()
    {
        auto bunnyObj = std::make_shared<GameObject>();
        auto bunnyTransform = std::make_shared<Transform>();

        bunnyObj->components.push_back(bunnyTransform);

        gameObjects.push_back(bunnyObj);
    }

    void TestInit2()
    {
        size_t n = 3;
        glm::vec3 pos[n] = {
            {0.0f, -1.0f, 0.0f},
            {-2.0f, -1.0f, -3.0f},
            {2.0f, -1.0f, -3.0f}
        };
        for (int i = 0; i < n; i++)
        {
            auto bunnyObj = std::make_shared<GameObject>();
            auto bunnyTransform = std::make_shared<Transform>(pos[i]);

            bunnyObj->components.push_back(bunnyTransform);

            gameObjects.push_back(bunnyObj);

        }

    }

    void TestInit3() {
        size_t n = 3;
        glm::vec3 pos[n] = {
            {0.0f, -1.0f, 0.0f},
            {-2.0f, -1.0f, -3.0f},
            {2.0f, -1.0f, -3.0f}
        };
        for (int i = 0; i < n; i++)
        {
            auto bunnyObj = std::make_shared<GameObject>();
            auto bunnyName = std::string("bunny" + std::to_string(randomID++));
            auto bunnyTransform = std::make_shared<Transform>(pos[i]);
            auto bunnyMaterial = std::make_shared<Material>();

            bunnyObj->name = bunnyName;
            bunnyObj->components.push_back(bunnyTransform);
            bunnyObj->components.push_back(bunnyMaterial);

            gameObjects.push_back(bunnyObj);
        }
    }

    void TestInit4()
    {

    }

    void AddGameObject(std::shared_ptr<GameObject>& gameObject)
    {
        gameObjects.push_back(gameObject);
    }

    void AddGameObject(){
        glm::vec3 pos= {0.0f, 0.0f, 0.0f};

        auto bunnyObj = std::make_shared<GameObject>();
        auto bunnyName = std::string("bunny" + std::to_string(randomID++));
        auto bunnyTransform = std::make_shared<Transform>(pos);
        auto bunnyMaterial = std::make_shared<Material>();

        bunnyObj->name = bunnyName;
        bunnyObj->components.push_back(bunnyTransform);
        bunnyObj->components.push_back(bunnyMaterial);

        AddGameObject(bunnyObj);

    };

    // Test Constructor
    GameEngine()
    {
        TestInit3();
    }

    const std::vector<std::shared_ptr<GameObject>>& GetGameObjects() { return gameObjects; }

};
#endif
