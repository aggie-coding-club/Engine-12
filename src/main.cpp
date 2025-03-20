#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>

#include "gui/gui_engine.h"
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <memory>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include "core/game_engine.h"
#include "render/render_engine.h"
#include "physics/physics_engine.h"


#include <yaml-cpp/yaml.h> // for tests, remove later
#include <serial/lights.h>
#include <serial/models.h>
#include "serial/project.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define NUM_LIGHTS 2
#define NUM_MATERIALS 3
#define NUM_SHADERS 3

GLFWwindow *window;

std::unique_ptr<GuiEngine> guiEngine;
std::unique_ptr<RenderEngine> renderEngine;
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
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);

	guiEngine = std::make_unique<GuiEngine>();
	renderEngine = std::make_unique<RenderEngine>(window, &gameEngine);
    physicsEngine = std::make_unique<PhysicsEngine>(&gameEngine, &timeDelta);

	guiEngine->init(window, &gameEngine);
    physicsEngine->Activate();
	while ( glfwWindowShouldClose(window) == 0 )
	{
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        start = end;
        physicsEngine->Update();

		guiEngine->run(width,height);
		if(guiEngine->showView) {
			renderEngine->Display();
		}
        end = std::chrono::steady_clock::now();
        timeDelta = end - start;
        std::cout << timeDelta.count() << std::endl;
		glfwSwapBuffers(window);
	}
	guiEngine->cleanup();

    glfwDestroyWindow(window);
    glfwTerminate();
	auto node = YAML::Node();
	node = SerializeProject(&gameEngine);
	std::ofstream yamlFile("../user/project.yaml");
	if(!yamlFile) {
		std::cout << "file does not exist" << std::endl;
		return 1;
	}
	yamlFile << node;
	yamlFile.close();



	return 0;
}