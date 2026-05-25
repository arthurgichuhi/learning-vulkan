#pragma once
#include "../EngineConfig.h"
#include "../EngineInstance.h"
#include "../logging.h"
#include "../device.h"
#include "../swapchain.h"

class Engine {
public:
	Engine();
	~Engine();

private:

	bool debugMode = true;

	int width{ 640 };
	int height{ 480 };

	std::string name{};

	GLFWwindow* window{ nullptr };
	vk::Instance instance{ nullptr };
	vk::DebugUtilsMessengerEXT debugMessenger{ nullptr };
	vk::detail::DispatchLoaderDynamic dldi;
	vk::SurfaceKHR surface;

	vk::PhysicalDevice physicalDevice{ nullptr };
	vk::Device device{ nullptr };
	vk::Queue graphicsQueue{ nullptr };
	vk::Queue presentQueue{ nullptr };
	vk::SwapchainKHR swapchain;
	std::vector<vkInit::SwapChainFrame> swapchainFrames;
	vk::Format swapchainFormat;
	vk::Extent2D swapchainExtent;

	void build_glfw_window();

	void make_instance();

	void make_device();
};