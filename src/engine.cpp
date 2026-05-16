#include "engine.h"
#include "instance.h"

namespace engine {

	Engine::Engine(){}

	Engine::~Engine() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void Engine::build_glfw_window() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		if (window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr)) {
			if (debugMode) {
				std::cout << "Successfully created GLFW window" << width << height;
		}
		}
		else {
			if (debugMode) {
				std::cout << "Failed to create GLFW window";
			}
		}


	}

	void Engine::make_instance() {
		instance = vkInit::make_instance(debugMode, name.c_str());
	}
}