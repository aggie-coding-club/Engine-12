#include "render/render_engine.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "render/tiny_obj_loader.h"

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

	ShadersInit();
}

void RenderEngine::LoadModel(const std::string &name)
{
    // Taken from Shinjiro Sueda with slight modification
    std::string meshName(name);
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string errStr;

    posBuffMap[meshName] = std::vector<float>();
    norBuffMap[meshName] = std::vector<float>();
    texBuffMap[meshName] = std::vector<float>(); // Assuming texBuffMap is declared somewhere

    std::vector<float>& posBuff = posBuffMap[meshName];
    std::vector<float>& norBuff = norBuffMap[meshName];
    std::vector<float>& texBuff = texBuffMap[meshName];

    bool rc = tinyobj::LoadObj(&attrib, &shapes, &materials, &errStr, meshName.c_str());
    if (!rc) {
        std::cerr << errStr << std::endl;
    }
    else {
        // Some OBJ files have different indices for vertex positions, normals,
        // and texture coordinates. For example, a cube corner vertex may have
        // three different normals. Here, we are going to duplicate all such
        // vertices.

        // Temporary storage for normals if not present in the OBJ file
        std::vector<glm::vec3> generatedNormals;

        // Loop over shapes
        for (auto & shape : shapes) {
            // Loop over faces (polygons)
            size_t index_offset = 0;
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
                size_t fv = shape.mesh.num_face_vertices[f];
                // Loop over vertices in the face.
                std::vector<glm::vec3> faceVertices;  // To store face vertices for normal calculation
                for (size_t v = 0; v < fv; v++) {
                    // Access to vertex
                    tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                    float vx = attrib.vertices[3 * idx.vertex_index + 0];
                    float vy = attrib.vertices[3 * idx.vertex_index + 1];
                    float vz = attrib.vertices[3 * idx.vertex_index + 2];
                    posBuff.push_back(vx);
                    posBuff.push_back(vy);
                    posBuff.push_back(vz);

                    // Store the vertex for normal calculation (if normals are absent)
                    faceVertices.push_back(glm::vec3(vx, vy, vz));

                    if (!attrib.normals.empty()) {
                        // If normals are already provided, add them
                        norBuff.push_back(attrib.normals[3 * idx.normal_index + 0]);
                        norBuff.push_back(attrib.normals[3 * idx.normal_index + 1]);
                        norBuff.push_back(attrib.normals[3 * idx.normal_index + 2]);
                    }

                    if (!attrib.texcoords.empty()) {
                        texBuff.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
                        texBuff.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
                    }
                }
                index_offset += fv;

                // Generate normals if not provided
                if (attrib.normals.empty()) {
                    glm::vec3 normal = GenerateNormal(faceVertices);
                    for (size_t v = 0; v < fv; v++) {
                        // Duplicate normal for each vertex in the face (flat shading)
                        norBuff.push_back(normal.x);
                        norBuff.push_back(normal.y);
                        norBuff.push_back(normal.z);
                    }
                }

                // per-face material (IGNORE)
                shape.mesh.material_ids[f];
            }
        }
    }
}

// Function to generate a normal for a face (using the cross product of two edges)
glm::vec3 RenderEngine::GenerateNormal(const std::vector<glm::vec3>& faceVertices)
{
    // Ensure there are exactly three vertices
    if (faceVertices.size() != 3) {
        std::cerr << "Error: Cannot generate normal for a non-triangle face." << std::endl;
        return glm::vec3(0.0f); // Return a zero normal
    }

    glm::vec3 v0 = faceVertices[0];
    glm::vec3 v1 = faceVertices[1];
    glm::vec3 v2 = faceVertices[2];

    // Calculate two edges
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;

    // Calculate the normal using the cross product
    glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

    return normal;
}


void RenderEngine::ShadersInit() {
	program.SetShadersFileName(shadersPath + verts[0],
            shadersPath + frags[0]);

	program.Init();

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
}


void RenderEngine::FrameBufferSizeCallback(GLFWwindow* lWindow, int width, int height)
{
	glViewport(0, 0, width, height);
}
