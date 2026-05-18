#include "engine.h"

namespace engine {

	Engine::Engine(){
		build_glfw_window();
		make_instance();
		make_debug_messenger();
	}

	Engine::~Engine() {
		instance.destroyDebugUtilsMessengerEXT(debugMessenger,nullptr,dldi);
		instance.destroy();
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

		instance = vkInit::make_instance(debugMode, "ID_TECH_12");
		dldi = vk::detail::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);
	}

	void Engine::make_debug_messenger() {
		debugMessenger = vkInit::make_debug_messenger(instance,dldi);
	}
}