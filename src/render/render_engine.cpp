#include "render/render_engine.h"

#include "core/components/component.h"
#include "core/components/light.h"
#include "core/components/transform.h"

#include <fmt/core.h>

void RenderEngine::Init()
{
#ifndef _USE_SCENE_
	// Lights
	lights[0].position = {0.0f, 0.0f, 3.0f};
	lights[0].color    = {0.5f, 0.5f, 0.5f};

	lights[1].position = {0.0f, 3.0f, 0.0f};
	lights[1].color    = {0.2f, 0.2f, 0.2f};
#endif
    glGenVertexArrays(1, &VAO); // Create a VAO
    glGenBuffers(1, &VBO);      // Create a VBO

    // Bind VAO
    glBindVertexArray(VAO);

    // Bind VBO and send vertex data to GPU
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute (location 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind VAO
    glBindVertexArray(0);

	ShadersInit();
}

void RenderEngine::Display()
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    // Other initialization...

    if (gameEngine->HasChangedScene())
    {
        // TODO reinitialize shadow maps
        gameEngine->ChangedSceneAcknowledged();
    }
    const auto& scene = gameEngine->GetCurrScene();
    camera = scene->GetCurrCamera();
    if (!camera) // Better check for null camera
    {
        std::cerr << "No camera available, skipping render." << std::endl;
        glDrawArrays(GL_POINTS, 0, 0);
        return;
    }

    glm::mat4 projectionMatrix = camera->GetProjectionMatrix();
    glm::mat4 viewMatrix = camera->GetViewMatrix();

    for (const auto& model : scene->GetModels())
    {
        const auto& objTransform = std::dynamic_pointer_cast<Transform>( 
                model->components[TRANSFORM] );
        const auto& objMaterial = std::dynamic_pointer_cast<Material>( 
                model->components[MATERIAL] );
        const auto& objModel = std::dynamic_pointer_cast<Model>(
                model->components[MODEL] );

        std::string& modelPath = objModel->modelPath;

        // Check if the position buffer is already loaded
        if (posBuffMap.find(modelPath) == posBuffMap.end()) {
            // Load model buffers if they are not already loaded
            LoadModel(modelPath);
        }

        glm::mat4 modelMatrix(1.0f);
        modelMatrix = glm::translate(glm::mat4(1.0f), objTransform->position)
            * glm::rotate(glm::mat4(1.0f), glm::radians(objTransform->rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f))
            * glm::rotate(glm::mat4(1.0f), glm::radians(objTransform->rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f))
            * glm::rotate(glm::mat4(1.0f), glm::radians(objTransform->rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f))
            * glm::scale(glm::mat4(1.0f), objTransform->scale);

        glm::mat4 modelInverseTranspose = glm::transpose(glm::inverse(modelMatrix));
        program.Bind();

        // Use existing buffers stored in posBuffMap
        program.SendAttributeData(posBuffMap[modelPath], "vPositionModel");
        program.SendAttributeData(norBuffMap[modelPath], "vNormalModel");
        program.SendUniformData(modelMatrix, "model");
        program.SendUniformData(viewMatrix, "view");
        program.SendUniformData(projectionMatrix, "projection");
        program.SendUniformData(modelInverseTranspose, "modelInverseTranspose");

        // Handle materials
        if (objMaterial) {
            program.SendUniformData(objMaterial->ambient, "ka");
            program.SendUniformData(objMaterial->diffuse, "kd");
            program.SendUniformData(objMaterial->specular, "ks");
            program.SendUniformData(objMaterial->shininess, "s");
        }

        // Handle lights
        const auto& lights = scene->GetLights();
        for (size_t i = 0; i < lights.size(); i++) {
            const auto& light = lights[i];
            const auto lightTransform = std::dynamic_pointer_cast<Transform>(light->components[TRANSFORM]);
            const auto lightComponent = std::dynamic_pointer_cast<Light>(light->components[LIGHT]);

            std::string name = fmt::format("lights[{}]", i);
            program.SendUniformData(lightTransform->position, (name + ".position").c_str());
            program.SendUniformData(lightComponent->color, (name+".color").c_str());
        }

        glDrawArrays(GL_TRIANGLES, 0, posBuffMap[modelPath].size() / 3);

        program.Unbind();
    }
}



void RenderEngine::CharacterCallback(GLFWwindow* window, unsigned int key)
{
	// Nothing rn cuz yea
}


void RenderEngine::FrameBufferSizeCallback(GLFWwindow* lWindow, int width, int height)
{
	glViewport(0, 0, width, height);
}
