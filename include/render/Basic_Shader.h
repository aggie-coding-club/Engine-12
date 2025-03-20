#pragma once

#include <cmath>

#include "render/tiny_obj_loader.h"

#include "render/Shader.h"

#include "core/components/component.h"
#include "core/components/light.h"
#include "core/components/transform.h"

struct Triangle
{
    alignas(16) glm::vec3 posA;
    alignas(16) glm::vec3 posB;
    alignas(16) glm::vec3 posC;
    alignas(16) glm::vec3 normalA;
    alignas(16) glm::vec3 normalB;
    alignas(16) glm::vec3 normalC;
    alignas(8) glm::vec2 uvA;
    alignas(8) glm::vec2 uvB;
    alignas(8) glm::vec2 uvC;
};

struct ModelInfo
{
    alignas(4) int nodeOffset;
    alignas(4) int triOffset;
    alignas(4) float pad1, pad2;
    alignas(16) glm::mat4x4 worldToLocalMatrix;
    alignas(16) glm::mat4x4 localToWorldMatrix;
    alignas(16) MaterialInfo material;
};

struct BVHNode
{
    alignas(16) glm::vec3 boundsMin;
    alignas(16) glm::vec3 boundsMax;
    alignas(4) int startIndex;
    alignas(4) int triangleCount;
};

class Basic_Shader final : public Shader
{

    std::shared_ptr<Camera> camera;
    std::vector<Triangle> triangles;
    std::vector<int> triIndexs;
    std::vector<ModelInfo> models;
    std::unordered_map<std::string, std::pair<unsigned int, unsigned int>> loadedMeshOffset;
    std::unordered_map<std::string, int> modelsLoaded;
    std::vector<BVHNode> bvhNodes;

    static float NodeCost(const glm::vec3 size,const int triNum)
    {
        const float area = size.x * size.y + size.x * size.z + size.y * size.z;
        return area * static_cast<float>(triNum);
    }

    struct BoundingBox
    {
        glm::vec3 Min{};
        glm::vec3 Max{};
        [[nodiscard]] glm::vec3 Center() const { return (Min + Max) / 2.f; }
        [[nodiscard]] glm::vec3 Size() const { return Max - Min; }
        bool hasPoint = false;

        void GrowToInclude(const Triangle &tri)
        {
            if (hasPoint)
            {
                Min = glm::min(Min, glm::min(tri.posA, glm::min(tri.posB, tri.posC)));
                Max = glm::max(Max, glm::max(tri.posA, glm::max(tri.posB, tri.posC)));
            }
            else
            {
                hasPoint = true;
                Min = glm::min(tri.posA,min(tri.posB, tri.posC));
                Max = glm::max(tri.posA,max(tri.posB, tri.posC));
            }
        }
    };

    static float SurfaceArea(const BoundingBox& box)
    {
        glm::vec3 size = box.Size();
        return 2.0f * (size.x * size.y + size.x * size.z + size.y * size.z);
    }


    static glm::vec3 GetTriCenter(Triangle &tri)
    {
        glm::vec3 center = tri.posA + tri.posB + tri.posC;
        return center / 3.f;
    }

    float EvaluateSplit(int splitAxis, float splitPos, int triStartIndex, int triCount)
    {
        BoundingBox boundsLeft, boundsRight;
        int numOnLeft = 0;
        int numOnRight = 0;

        for (int i = triStartIndex; i < triStartIndex + triCount; ++i)
        {
            if (GetTriCenter(triangles[triIndexs[i]])[splitAxis] < splitPos)
            {
                boundsLeft.GrowToInclude(triangles[triIndexs[i]]);
                ++numOnLeft;
            }
            else
            {
                boundsRight.GrowToInclude(triangles[triIndexs[i]]);
                ++numOnRight;
            }
        }

        if (numOnLeft == 0 || numOnRight == 0)
        {
            return std::numeric_limits<float>::infinity(); // Avoid empty splits
        }

        const float SA_P = SurfaceArea(BoundingBox{boundsLeft.Min, boundsRight.Max}); // Parent surface area
        const float SA_L = SurfaceArea(boundsLeft);
        const float SA_R = SurfaceArea(boundsRight);

        constexpr float C_T = 1.0f; // Traversal cost
        constexpr float C_I = 1.0f; // Intersection cost

        const float cost = C_T + ((SA_L / SA_P) * static_cast<float>(numOnLeft) * C_I) + ((SA_R / SA_P) * static_cast<float>(numOnRight) * C_I);
        return cost;
    }


    struct SplitCalc
    {
        int axis;
        float pos;
        float cost;
    };

    SplitCalc ChooseSplit(const BVHNode &node, int triStartPos, int triCount)
    {
        if (triCount <= 1)
        {
            return SplitCalc{0, 0, std::numeric_limits<float>::infinity()};
        }

        float bestSplitPos = 0;
        int bestSplitAxis = 0;
        const int numSplitTests = 5;

        float bestCost = std::numeric_limits<float>::infinity();

        for (int axis = 0; axis < 3; ++axis)
        {
            for (int i = 0; i < numSplitTests; ++i)
            {
                const float splitT = (static_cast<float>(i) + 1.f) / (numSplitTests + 1.f);
                const float splitPos = node.boundsMin[axis] + ((node.boundsMax[axis] - node.boundsMin[axis]) * splitT);
                const float cost = EvaluateSplit(axis, splitPos, triStartPos, triCount);

                if (cost < bestCost)
                {
                    bestCost = cost;
                    bestSplitPos = splitPos;
                    bestSplitAxis = axis;
                }
            }
        }

        return SplitCalc{bestSplitAxis, bestSplitPos, bestCost};
    }

    void Split(int parentIndex, int triStartIndex, int triCount, int depth = 0)
    {
        constexpr int MaxDepth = 64;
        BVHNode parent = bvhNodes[parentIndex];
        glm::vec3 size = parent.boundsMax - parent.boundsMin;
        float parentCost = NodeCost(size, triCount);

        auto [axis, pos, cost] = ChooseSplit(parent, triStartIndex, triCount);

        if (depth < MaxDepth && cost < parentCost)
        {
            BoundingBox boundsLeft, boundsRight;
            int numOnLeft = 0;

            for (int i = triStartIndex; i < triStartIndex + triCount; ++i)
            {
                if(GetTriCenter(triangles[triIndexs[i]])[axis] < pos)
                {
                    int temp = triIndexs[i];
                    triIndexs[i] = triIndexs[triStartIndex + numOnLeft];
                    triIndexs[triStartIndex + numOnLeft] = temp;

                    boundsLeft.GrowToInclude(triangles[triIndexs[i]]);
                    ++numOnLeft;
                }
                else
                {
                    boundsRight.GrowToInclude(triangles[triIndexs[i]]);
                }
            }

            int numOnRight = triCount - numOnLeft;

            int triStartLeft = triStartIndex + 0;
            int triStartRight = triStartIndex + numOnLeft;

            int childIndexLeft = (int)bvhNodes.size();
            bvhNodes.push_back(BVHNode{boundsLeft.Min, boundsLeft.Max, triStartLeft, 0});
            int childIndexRight = (int)bvhNodes.size();
            bvhNodes.push_back(BVHNode{boundsRight.Min, boundsRight.Max, triStartRight, 0});

            parent.startIndex = childIndexLeft;
            bvhNodes[parentIndex] = parent;

            Split(childIndexLeft, triStartIndex, numOnLeft, depth + 1);
            Split(childIndexRight, triStartIndex + numOnLeft, numOnRight, depth + 1);
        }
        else
        {
            parent.startIndex = triStartIndex;
            parent.triangleCount = triCount;
            bvhNodes[parentIndex] = parent;
        }
    }

    void BuildBVH(int triStartIndex, int triCount)
    {
        BVHNode root{glm::vec3(std::numeric_limits<float>::infinity()),
                     glm::vec3(-std::numeric_limits<float>::infinity()),
                     -1, -1};

        for(size_t i = 0; i < triCount; i++)
        {
            root.boundsMin = glm::min(root.boundsMin, triangles[triStartIndex + i].posA);
            root.boundsMin = glm::min(root.boundsMin, triangles[triStartIndex + i].posB);
            root.boundsMin = glm::min(root.boundsMin, triangles[triStartIndex + i].posC);
            root.boundsMax = glm::max(root.boundsMax, triangles[triStartIndex + i].posA);
            root.boundsMax = glm::max(root.boundsMax, triangles[triStartIndex + i].posB);
            root.boundsMax = glm::max(root.boundsMax, triangles[triStartIndex + i].posC);
        }
        bvhNodes.push_back(root);
        Split(static_cast<int>(bvhNodes.size()) - 1, triStartIndex, triCount);
    }

    // Function to generate a normal for a face (using the cross product of two edges)
    static glm::vec3 GenerateNormal(const std::vector<glm::vec3>& faceVertices)
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

    void LoadModel(const std::string &name)
    {
        // Taken from Shinjiro Sueda with slight modification
        const std::string& meshName(name);
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string errStr;

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
            loadedMeshOffset[meshName].first = triangles.size();

            // Loop over shapes
            for (auto & shape : shapes) {
                // Loop over faces (polygons)
                size_t index_offset = 0;
                for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
                    size_t fv = shape.mesh.num_face_vertices[f];
                    Triangle tri{};
                    std::vector<glm::vec3> position;
                    std::vector<glm::vec3> normal;
                    std::vector<glm::vec2> uv;
                    // Loop over vertices in the face.
                    std::vector<glm::vec3> faceVertices;  // To store face vertices for normal calculation
                    for (size_t v = 0; v < fv; v++) {
                        // Access to vertex
                        tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                        float vx = attrib.vertices[3 * idx.vertex_index + 0];
                        float vy = attrib.vertices[3 * idx.vertex_index + 1];
                        float vz = attrib.vertices[3 * idx.vertex_index + 2];
                        position.push_back(glm::vec3(vx, vy, vz));

                        // Store the vertex for normal calculation (if normals are absent)
                        faceVertices.push_back(glm::vec3(vx, vy, vz));

                        if (!attrib.normals.empty()) {
                            // If normals are already provided, add them
                            // norBuff.push_back(attrib.normals[3 * idx.normal_index + 0]);
                            // norBuff.push_back(attrib.normals[3 * idx.normal_index + 1]);
                            // norBuff.push_back(attrib.normals[3 * idx.normal_index + 2]);
                            glm::vec3 tempnormal(attrib.normals[3 * idx.normal_index + 0], attrib.normals[3 * idx.normal_index + 1], attrib.normals[3 * idx.normal_index + 2]);
                            normal.push_back(tempnormal);
                        }

                        if (!attrib.texcoords.empty()) {
                            // texBuff.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
                            // texBuff.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
                            uv.push_back(glm::vec2(attrib.texcoords[2 * idx.texcoord_index + 0], attrib.texcoords[2 * idx.texcoord_index + 1]));
                        }
                    }

                    index_offset += fv;
                    // Generate normals if not provided
                    if (attrib.normals.empty()) {
                        glm::vec3 tempnormal = GenerateNormal(faceVertices);
                        for (size_t v = 0; v < fv; v++) {
                            // Duplicate normal for each vertex in the face (flat shading)
                            // norBuff.push_back(normal.x);
                            // norBuff.push_back(normal.y);
                            // norBuff.push_back(normal.z);
                            normal.push_back(tempnormal);
                        }
                    }

                    tri.posA = position[0];
                    tri.posB = position[1];
                    tri.posC = position[2];

                    tri.normalA = normal[0];
                    tri.normalB = normal[1];
                    tri.normalC = normal[2];

                    if(!uv.empty())
                    {
                        tri.uvA = uv[0];
                        tri.uvB = uv[1];
                        tri.uvC = uv[2];
                    }
                    else
                    {
                        tri.uvA = glm::vec2(0);
                        tri.uvB = tri.uvA;
                        tri.uvC = tri.uvB;
                    }

                    triangles.push_back(tri);
                    triIndexs.push_back(static_cast<int>(triIndexs.size()));
                }
            }
        }
    }

    template <typename T>
    void SendBufferData(std::vector<T>& buffer, const char* name, GLuint bindingIndex) {
        if (buffer.empty()) {
            std::cerr << "Empty buffer for attribute: " << name << std::endl;
            return;
        }

        GLuint bufferID = 0;

        // Check if the buffer exists for the given name
        if (bufferMap.find(name) == bufferMap.end()) {
            glGenBuffers(1, &bufferID);  // Generate a new buffer
            bufferMap[name] = bufferID;  // Store the buffer ID in the map
        } else {
            bufferID = bufferMap[name];  // Reuse the existing buffer ID
        }

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);  // Bind SSBO
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(T) * buffer.size(), buffer.data(), GL_STATIC_DRAW);  // Upload data
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, bufferID);  // Bind to specific binding point
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);  // Unbind
    }

public:

    static void PrintVec3(glm::vec3 print)
    {
        std::cout << "(" << print[0] << ", " << print[1] << ", " << print[2] << ")" << std::endl;
    }

    void UpdateData() override {
        camera = scene->GetCurrCamera();
        if (!camera) // Check for a null camera
        {
            std::cerr << "No camera available, skipping render." << std::endl;
            glDrawArrays(GL_POINTS, 0, 0);
            return;
        }

        // Send Camera world space position
        SendUniformData(camera->GetPosition(), "_WorldSpaceCamPos");

        // Compute Camera's Local-to-World matrix (inverse of ViewMatrix)
        glm::mat4 CamLocalToWorldMatrix = glm::inverse(camera->GetViewMatrix());
        SendUniformData(CamLocalToWorldMatrix, "camLocalToWorldMatrix");

        // Compute the height and width of the view plane
        float fovy = glm::clamp(camera->GetFovy(), 1.0f, 179.0f); // Clamping FOV for safety
        float focusDist = camera->GetFocusDist();
        float aspect = camera->GetAspect();

        float planeHeight = focusDist * std::tan(glm::radians(fovy) * 0.5f) * 2.0f;
        float planeWidth = planeHeight * aspect;

        int useSun = 1;
        float sunFocus = 0.1;
        float sunIntensity = 0.5;
        glm::vec3 sunColor = glm::vec3(1,1,1);

        for (const auto& model : scene->GetModels())
        {
            if(modelsLoaded.find(model->name) == modelsLoaded.end())
            {
                const auto& objTransform = std::dynamic_pointer_cast<Transform>( model->components[TRANSFORM]);
                const auto& objMaterial = std::dynamic_pointer_cast<Material>( model->components[MATERIAL]);
                const auto& objModel = std::dynamic_pointer_cast<Model>(model->components[MODEL]);

                std::string& modelPath = objModel->modelPath;

                // Check if the position buffer is already loaded
                if (loadedMeshOffset.find(modelPath) == loadedMeshOffset.end()) {
                    // Load model buffers if they are not already loaded
                    LoadModel(modelPath);
                    int offset = static_cast<int>(loadedMeshOffset[modelPath].first);
                    loadedMeshOffset[modelPath].second = bvhNodes.size();
                    BuildBVH(offset, static_cast<int>(triangles.size()) - offset);
                }

                glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), objTransform->position)
                      * glm::rotate(glm::mat4(1.0f), glm::radians(objTransform->rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f))
                      * glm::rotate(glm::mat4(1.0f), glm::radians(objTransform->rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f))
                      * glm::rotate(glm::mat4(1.0f), glm::radians(objTransform->rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f))
                      * glm::scale(glm::mat4(1.0f), objTransform->scale);

                ModelInfo currentModel{};
                currentModel.triOffset = static_cast<int>(loadedMeshOffset[modelPath].first);
                currentModel.nodeOffset = static_cast<int>(loadedMeshOffset[modelPath].second);
                currentModel.localToWorldMatrix = modelMatrix;
                currentModel.worldToLocalMatrix = glm::inverse(modelMatrix);
                currentModel.material = objMaterial->getMaterial();

                modelsLoaded[model->name] = static_cast<int>(models.size());

                models.push_back(currentModel);
            }
            else
            {
                const auto& objTransform = std::dynamic_pointer_cast<Transform>( model->components[TRANSFORM]);
                const auto& objMaterial = std::dynamic_pointer_cast<Material>( model->components[MATERIAL]);
                const auto& objModel = std::dynamic_pointer_cast<Model>(model->components[MODEL]);

                glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), objTransform->position)
                      * glm::rotate(glm::mat4(1.0f), glm::radians(objTransform->rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f))
                      * glm::rotate(glm::mat4(1.0f), glm::radians(objTransform->rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f))
                      * glm::rotate(glm::mat4(1.0f), glm::radians(objTransform->rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f))
                      * glm::scale(glm::mat4(1.0f), objTransform->scale);

                ModelInfo currentModel{};
                currentModel.triOffset = static_cast<int>(loadedMeshOffset[objModel->modelPath].first);
                currentModel.nodeOffset = static_cast<int>(loadedMeshOffset[objModel->modelPath].second);
                currentModel.localToWorldMatrix = modelMatrix;
                currentModel.worldToLocalMatrix = glm::inverse(modelMatrix);
                currentModel.material = objMaterial->getMaterial();

                models[modelsLoaded[model->name]] = currentModel;
            }
            /* Handle lights
            const auto& lights = scene->GetLights();
            for (size_t i = 0; i < lights.size(); i++) {
                const auto& light = lights[i];
                const auto lightTransform = std::dynamic_pointer_cast<Transform>(light->components[TRANSFORM]);
                const auto lightComponent = std::dynamic_pointer_cast<Light>(light->components[LIGHT]);

                std::string name = fmt::format("lights[{}]", i);
                this->SendUniformData(lightTransform->position, (name + ".position").c_str());
                this->SendUniformData(lightComponent->color, (name+".color").c_str());
            } */
        }

        // Send viewParams as (width, height, focus distance)
        SendUniformData(glm::vec3(planeWidth, planeHeight, focusDist), "viewParams");

        // Send environment params
        SendUniformData(useSun, "useSky");
        SendUniformData(sunFocus, "sunFocus");
        SendUniformData(sunIntensity, "sunIntensity");
        SendUniformData(sunColor, "sunColor");

        // Send Model and BVH info
        SendUniformData((int)models.size(), "modelCount");
        SendBufferData(models, "models", 0);
        SendUniformData((int)triangles.size(), "triangleCount");
        SendBufferData(triangles, "triangles", 1);
        SendBufferData(triIndexs, "triIndex", 4);
        SendBufferData(bvhNodes, "nodes", 2);
    };
};