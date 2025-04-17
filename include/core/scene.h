#pragma once

#include <iostream>
#include <ostream>

#include "core/game_object.h"
#include "core/camera.h"
#include "core/components/transform.h"
#include "core/components/model.h"
#include "core/components/material.h"
#include "core/components/rigidbody.h"

#include <vector>

#include "fastgltf/types.hpp"
#include "fmt/chrono.h"

class Scene {
private:
    std::vector<std::shared_ptr<GameObject>> 	models;
    std::vector<std::shared_ptr<GameObject>> 	lights;
    std::vector<std::shared_ptr<Camera>> 		cameras;

	// Directional Light Info

	float lightNearPlane = 0.1f, lightFarPlane = 50.f;
	glm::vec3 lightEye = glm::vec3(-10.0f,4.f,5.f);
	glm::vec3 lightCenter = glm::vec3(0.0f);
	glm::vec3 lightUp = glm::vec3(0.0f, 1.0f, 0.0f);

	// Directional Light Matrices
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, lightNearPlane, lightFarPlane);
	glm::mat4 lightView = glm::lookAt(lightEye, lightCenter, lightUp);
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    std::string name;
    int currCameraIdx = 0;

    std::string modelPath = "../resources/models/";

    static int randomId;
public:

	inline float getLightNearPlane() const { return lightNearPlane; }
	inline float getLightFarPlane() const { return lightFarPlane; }
	inline glm::vec3 getlightEye() const { return lightEye; }
	inline glm::vec3 getlightCenter() const { return lightCenter; }
	inline glm::vec3 getlightUp() const { return lightUp; }

	inline glm::mat4 getLightProjection() const { return lightProjection; }
	inline glm::mat4 getLightView() const { return lightView; }
	inline glm::mat4 getLightSpaceMatrix() const { return lightSpaceMatrix; }

	void setLightNearPlane(const float &nearPlane) {
		lightNearPlane = nearPlane;
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, lightNearPlane, lightFarPlane);
		lightSpaceMatrix = lightProjection * lightView;
	}
	void setLightFarPlane(const float &farPlane) {
		lightFarPlane = farPlane;
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, lightNearPlane, lightFarPlane);
		lightSpaceMatrix = lightProjection * lightView;
	}

	void setLightEye(const glm::vec3 &eye) {
		lightEye = eye;
		lightView = glm::lookAt(lightEye, lightCenter, lightUp);
		lightSpaceMatrix = lightProjection * lightView;
	}

	void setLightCenter(const glm::vec3 &center) {
		lightCenter = center;
		lightView = glm::lookAt(lightEye, lightCenter, lightUp);
		lightSpaceMatrix = lightProjection * lightView;
	}

	void setLightUp(const glm::vec3 &up) {
		lightUp = up;
		lightView = glm::lookAt(lightEye, lightCenter, lightUp);
		lightSpaceMatrix = lightProjection * lightView;
	}

    std::shared_ptr<GameObject> selectedGameObj = nullptr;
    bool mOrL; // Model or Light object

    std::shared_ptr<Camera> selectedCamera = nullptr;

	int current_shader = 1;

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
        glm::vec3 pos= {0.0f, 0.0f, 0.0f};

        const auto bunnyObj = std::make_shared<GameObject>();
        const auto bunnyName = std::string("bunny" + std::to_string(randomId++));
        const auto bunnyTransform = std::make_shared<Transform>(pos);
        const auto bunnyMaterial = std::make_shared<Material>();
  	    const auto bunnyModel = std::make_shared<Model>();
        const auto bunnyRigid = std::make_shared<RigidBody>();

        bunnyObj->name = bunnyName;
        bunnyObj->components[TRANSFORM] = bunnyTransform;
        bunnyObj->components[MATERIAL] = bunnyMaterial;
  	    bunnyObj->components[MODEL] = bunnyModel;
  	    bunnyObj->components[RIGID_BODY] = bunnyRigid;
        

  	    std::cout << std::dynamic_pointer_cast<Model>(bunnyObj->components[MODEL])->modelPath << std::endl;

        AddModel(bunnyObj);
    };

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


	// returns a vector of game objects containing specific tag
	std::vector<std::shared_ptr<GameObject>> SearchByTag(const std::string& tag) {
		std::vector<std::shared_ptr<GameObject>> gameObjects;

		for(std::shared_ptr<GameObject> model : models) {
			if(model->CompareTag(tag)) {
				gameObjects.push_back(model);
			}
		}

		for(std::shared_ptr<GameObject> light : lights) {
			if(light->CompareTag(tag)) {
				gameObjects.push_back(light);
			}
		}

		return gameObjects;
	}

	// returns a vector of game objects containing specific component
	std::vector<std::shared_ptr<GameObject>> SearchByComponent(COMPONENT_TYPE type) {
		std::vector<std::shared_ptr<GameObject>> gameObjects;

		for(std::shared_ptr<GameObject> model : models) {
			if(model->GetComponent(type)) {
				gameObjects.push_back(model);
			}
		}

		for(std::shared_ptr<GameObject> light : lights) {
			if(light->GetComponent(type)) {
				gameObjects.push_back(light);
			}
		}

		return gameObjects;
	}

	// returns a vector of game objects with specific name
	std::vector<std::shared_ptr<GameObject>> SearchByName(const std::string& name) {
		std::vector<std::shared_ptr<GameObject>> gameObjects;

		for(std::shared_ptr<GameObject> model : models) {
			if(model->name == name) {
				gameObjects.push_back(model);
			}
		}

		for(std::shared_ptr<GameObject> light : lights) {
			if(light->name == name) {
				gameObjects.push_back(light);
			}
		}

		return gameObjects;
	}

	// returns a vector of game objects with a name that contains a given sub-name
	std::vector<std::shared_ptr<GameObject>> SearchByNameContains(const std::string& name) {
		std::vector<std::shared_ptr<GameObject>> gameObjects;

		for(std::shared_ptr<GameObject> model : models) {
			if(model->name.find(name) != std::string::npos) {
				gameObjects.push_back(model);
			}
		}

		for(std::shared_ptr<GameObject> light : lights) {
			if(light->name.find(name) != std::string::npos) {
				gameObjects.push_back(light);
			}
		}

		return gameObjects;
	}

};

