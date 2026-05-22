#include "engine.h"

Engine::Engine() {
	build_glfw_window();
	make_instance();
	make_device();
}

Engine::~Engine() {
	device.destroy();
	instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldi);
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
	if (debugMode) {
		debugMessenger = vkInit::make_debug_messenger(instance, dldi);
	}

}

void Engine::make_device() {
	physicalDevice = vkInit::choose_physical_device(instance, debugMode);
	device = vkInit::create_logical_device(physicalDevice, debugMode);
	graphicsQueue = vkInit::get_queue(physicalDevice, device, debugMode);
	//vkInit::findQueueFamilies(physicalDevice, debugMode);
}