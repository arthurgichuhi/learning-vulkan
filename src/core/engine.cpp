#include "engine.h"
#include "../pipeline.h"

Engine::Engine() {
	build_glfw_window();
	make_instance();
	make_device();
	make_pipeline();
}

Engine::~Engine() {

	device.destroyPipeline(pipeline);
	device.destroyPipelineLayout(layout);
	device.destroyRenderPass(renderPass);

	for (auto frame : swapchainFrames) {
		device.destroyImageView(frame.imageView);
	}
	device.destroySwapchainKHR(swapchain);
	device.destroy();
	instance.destroySurfaceKHR(surface);
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
	VkSurfaceKHR c_style_surface;
	if (glfwCreateWindowSurface(instance, window, nullptr, &c_style_surface) != VK_SUCCESS) {
		if (debugMode) {
			std::cout << "\n" << "Failed to abstract glfw surface for vulkan"<<"\n";
		}
	}
	else {
		if (debugMode) {
			std::cout << "\n" << "Successful abstraction of glfw surface for vulkan" << "\n";
		}
		surface = c_style_surface;
	}

}

void Engine::make_device() {
	physicalDevice = vkInit::choose_physical_device(instance, debugMode);
	device = vkInit::create_logical_device(physicalDevice, surface, debugMode);
	auto queues = vkInit::get_queue(physicalDevice, device, surface, debugMode);
	graphicsQueue = queues[0];
	presentQueue = queues[1];
	auto bundle = vkInit::create_swapchain(device, physicalDevice, surface, width, height, debugMode);
	swapchain = bundle.swapChain;
	swapchainFrames = bundle.frames;
	swapchainFormat = bundle.format;
	swapchainExtent = bundle.extent;
}

void Engine::make_pipeline() {
	vkInit::GraphicsPipelineInBundle specification = {};

	specification.device = device;
	specification.vertexFilepath = "./../../../src/shaders/vertex.spv";
	specification.fragmentFilePath = "./../../../src/shaders/fragment.spv";
	specification.swapchainExtent = swapchainExtent;
	specification.swapchainImageFormat = swapchainFormat;

	auto output = vkInit::make_graphics_pipeline(specification, debugMode);
	layout = output.layout;
	renderPass = output.renderPass;
	pipeline = output.pipeline;

}