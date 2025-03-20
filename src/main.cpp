
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>

#include "gui/gui_engine.h"
#define TINYOBJLOADER_IMPLEMENTATION
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <memory>
#include <filesystem>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include "core/game_engine.h"
#include "render/render_engine.h"
#include "physics/physics_engine.h"

#include "gui/Terminal.h"

#include <yaml-cpp/yaml.h> // for tests, remove later
#include <serial/lights.h>
#include <serial/models.h>
#include "core/scene.h"
#include "scripting/scripting_engine.h"
#include "serial/scenes.h"
#include "serial/project.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

GLFWwindow *window;

std::unique_ptr<GuiEngine> guiEngine;
std::unique_ptr<RenderEngine> renderEngine;
std::unique_ptr<ScriptingEngine> scriptingEngine;
std::unique_ptr<PhysicsEngine> physicsEngine;

GameEngine gameEngine;

// Time delta calculations for physics
std::chrono::duration<float> timeDelta = std::chrono::duration<float>::zero();
auto start = std::chrono::steady_clock::now();
auto end   = std::chrono::steady_clock::now();

// Keyboard character callback function
void CharacterCallback(GLFWwindow* lWindow, unsigned int key)
{
	renderEngine->CharacterCallback(lWindow, key);
}
void FrameBufferSizeCallback(GLFWwindow* lWindow, int width, int height)
{
	renderEngine->FrameBufferSizeCallback(lWindow, width, height);
}


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main(int argc, char *argv[])
{	

	DeserializeProject(&gameEngine, "../user/project.yaml");

	
	// GLFWwindow* window is shared between gui and render,
	// so let's declare it in main.
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
	    return -1;
    }
	glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "TBDEngine", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glfwSetCharCallback(window, CharacterCallback);
	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);

	// Creates the assets folder if it doesn't already exist
	if (!std::filesystem::exists(std::filesystem::current_path() / "Assets")) {
		std::filesystem::create_directory(std::filesystem::current_path() / "Assets");
	}

	guiEngine = std::make_unique<GuiEngine>();
	renderEngine = std::make_unique<RenderEngine>(window, &gameEngine);
	scriptingEngine = std::make_unique<ScriptingEngine>();
	scriptingEngine->init();
  physicsEngine = std::make_unique<PhysicsEngine>(&gameEngine, &timeDelta);

	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// Create a texture to attach to the framebuffer
	unsigned int textureColorbuffer;
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

	// Create a renderbuffer for depth and stencil attachment (optional)
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// Check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "ERROR: Framebuffer is not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	int width,height;
	guiEngine->init(window, &gameEngine);
    physicsEngine->Activate();
	while ( glfwWindowShouldClose(window) == 0 )
	{
		
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		unsigned int width = static_cast<unsigned int>(w);
		unsigned int height = static_cast<unsigned int>(h);

		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        start = end;
        physicsEngine->Update();

		guiEngine->run(width,height);
		if(guiEngine->showView) {
			renderEngine->Display(framebuffer, width, height);
		}
        end = std::chrono::steady_clock::now();
        timeDelta = end - start;
        std::cout << timeDelta.count() << std::endl;
		glfwSwapBuffers(window);
		scriptingEngine->runScriptUpdate();
	}
	guiEngine->cleanup();

    glfwDestroyWindow(window);
    glfwTerminate();
	auto node = YAML::Node();
	node = SerializeProject(&gameEngine);
	YAML::Node project = SerializeProject(&gameEngine);
	std::string filename = "../user/" + gameEngine.getName() + ".yaml";
	std::ofstream yamlFile(filename);
	if(!yamlFile) {
		std::cout << "file does not exist" << std::endl;
		return 1;
	}
	yamlFile << node;
	yamlFile.close();
	scriptingEngine->cleanUp();


	return 0;
}