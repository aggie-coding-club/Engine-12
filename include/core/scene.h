#pragma once

#include <iostream>
#include <ostream>

#include "core/game_object.h"
#include "core/camera.h"
#include "core/components/transform.h"
#include "core/components/model.h"
#include "core/components/material.h"
#include "core/components/rigidbody.h"
#include "core/components/colliders/sphere_collider.h"
#include "core/components/colliders/plane_collider.h"

#include <vector>

#include "fastgltf/types.hpp"
#include "fmt/chrono.h"

class Scene {
private:
    std::vector<std::shared_ptr<GameObject>> 	models;
    std::vector<std::shared_ptr<GameObject>> 	lights;
    std::vector<std::shared_ptr<Camera>> 		cameras;

    std::string name;
    int currCameraIdx = 0;

    std::string modelPath = "../resources/models/";

    static int randomId;
public:
    std::shared_ptr<GameObject> selectedGameObj = nullptr;
    std::shared_ptr<Camera> selectedCamera = nullptr;
    bool mOrL;

    Scene() = default;
  	std::vector<std::shared_ptr<GameObject>>& 	GetModels()
    {
        return models;
    }
    std::vector<std::shared_ptr<GameObject>>& 	GetLights()
    {
        return lights;
    }

    std::vector<std::shared_ptr<Camera>>& 	    GetCameras()
  	{
  	    return cameras;
  	}

    std::shared_ptr<Camera> GetCurrCamera()
    {
  	    if (cameras.empty())
  	    {
	  	    return nullptr;
  	    }
        return cameras.at(currCameraIdx);
    }
    void AddModel(const std::shared_ptr<GameObject>& model)
    {
        models.push_back(model);
    }

    void AddModel(const std::string& objFile, const std::string& name)
  	{
  	    std::cout << objFile << std::endl;
  	    if (objFile.find(".obj") == std::string::npos)
  	    {
  	        std::cerr << "objFile is not of filetype .obj" << std::endl;
  	        assert(false);
  	    }

        glm::vec3 pos= {0.0f, 0.0f, 0.0f};

        auto obj = std::make_shared<GameObject>();
  	    const auto objName = objFile.substr(0, objFile.find_last_of('.')) + std::to_string(randomId++);

        const auto objTransform = std::make_shared<Transform>(pos);
        const auto objMaterial = std::make_shared<Material>();
  	    const auto objModel = std::make_shared<Model>(objFile);

        obj->name = objName;
        obj->components[TRANSFORM] = objTransform;
        obj->components[MATERIAL] = objMaterial;
  	    obj->components[MODEL] = objModel;


        AddModel(obj);
  	}

    void AddModel(){
        glm::vec3 pos = {0.0f, 0.0f, 0.0f};

        const auto bunnyObj = std::make_shared<GameObject>();
        const auto bunnyName = std::string("bunny" + std::to_string(randomId++));
        const auto bunnyTransform = std::make_shared<Transform>(pos);
        const auto bunnyMaterial = std::make_shared<Material>();
  	    const auto bunnyModel = std::make_shared<Model>();
        const auto bunnyRigid = std::make_shared<RigidBody>();
        const auto bunnyCollider = std::make_shared<SphereCollider>();

        bunnyRigid->coeff_e = 0.9f;

        bunnyCollider->point = bunnyTransform->position;
        bunnyCollider->radius = 1.0f;

        bunnyObj->name = bunnyName;
        bunnyObj->components[TRANSFORM] = bunnyTransform;
        bunnyObj->components[MATERIAL] = bunnyMaterial;
  	    bunnyObj->components[MODEL] = bunnyModel;
  	    bunnyObj->components[RIGID_BODY] = bunnyRigid;
        bunnyObj->components[COLLIDER] = bunnyCollider;

  	    std::cout << std::dynamic_pointer_cast<Model>(bunnyObj->components[MODEL])->modelPath << std::endl;

        AddModel(bunnyObj);
    };

    void AddPlaneCollider() {
        glm::vec3 pos = {0.0f, -1.25f, 0.0f};
        glm::vec3 normal = {0.0f, 1.0f, 0.0f};

        const auto planeObj = std::make_shared<GameObject>();
        const auto name = "plane";
        const auto planeTransform = std::make_shared<Transform>(pos);
        const auto planeRigid = std::make_shared<RigidBody>();
        const auto planeCollider = std::make_shared<PlaneCollider>(pos, normal);

        planeRigid->useGravity = false;
        planeRigid->isKinematics = false;

        planeObj->components[TRANSFORM] = planeTransform;
        planeObj->components[RIGID_BODY] = planeRigid;
        planeObj->components[COLLIDER] = planeCollider;
        planeObj->name = name;

        AddModel(planeObj);
    }


    void SetLightsVector(const std::vector<std::shared_ptr<GameObject>> _lights) {
        lights = _lights;
    }

    void SetModelsVector(const std::vector<std::shared_ptr<GameObject>> _models) {
        models = _models;
    }

    std::string& GetName() {
        return name;
    }

    void SetName(const std::string& _name) {
        name = _name;
    }

	void ResetCurrCameraIdx(){ currCameraIdx = 0; };

	void SetCurrCameraIdx(int i){ currCameraIdx = i; };
};

