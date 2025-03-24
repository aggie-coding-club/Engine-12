#pragma once

#include "shader.h"
#include "core/camera.h"
#include "core/game_engine.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define NUM_LIGHTS 2
#define NUM_MATERIALS 3
#define NUM_SHADERS 3

class RenderEngine
{
	GLFWwindow* window;
	const std::string modelPath = "../resources/models/";
	const std::string shadersPath = "../resources/shaders/";

	std::string verts[NUM_SHADERS] = {
		"phong_vert.glsl",
		"shadow_vert.glsl"
	};

	std::string frags[NUM_SHADERS] = {
		"phong_frag.glsl",
		"shadow_frag.glsl"
	};

	Shader program;
	Shader shadow;
	std::unordered_map<std::string, std::vector<float>> posBuffMap;
	std::unordered_map<std::string, std::vector<float>> texBuffMap;
	std::unordered_map<std::string, std::vector<float>> norBuffMap;

	std::vector<float> posBuff;
	std::vector<float> norBuff;
	std::vector<float> texBuff;

	struct lightStruct {
		glm::vec3 position;
		glm::vec3 color;
	} lights[NUM_LIGHTS];

	int mat_idx = 0;
	int shader_idx = 0;

	GLuint shaderProgram;
	GLuint shadowProgram;

	std::shared_ptr<Camera> camera;
	GameEngine* gameEngine;

	// Helper Function to generate normals when obj doesn't have normals saved in the file
	glm::vec3 GenerateNormal(const std::vector<glm::vec3>& faceVertices);

public:
	RenderEngine(GLFWwindow* _window, GameEngine* _gameEngine)
	{
		window = _window;
		gameEngine = _gameEngine;

		Init();
	}

	void SetProgram(GLuint _shaderProgram, GLuint _shadowProgram) {
		shaderProgram = _shaderProgram;
		shadowProgram = _shadowProgram;
	};

	void Init();

	void ShadersInit();

	void MapShadows(GLuint depthMapFBO, GLuint shadowWidth, GLuint shadowHeight);

	void Display(glm::vec4 viewportInfo);
	void CharacterCallback(GLFWwindow* window, unsigned int key);
	void FrameBufferSizeCallback(GLFWwindow* lWindow, int width, int height);

	void LoadModel(const std::string &name);

};