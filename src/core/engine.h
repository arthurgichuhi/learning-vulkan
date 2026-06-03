#pragma once
#include "../EngineConfig.h"
#include "../EngineInstance.h"
#include "../frame.h"
#include "scene.h"

class Engine {
public:
	Engine(int width, int height, GLFWwindow* window, bool debug);
	~Engine();

	void render(Scene* scene);

private:

	bool debugMode;

	int width, height;

	std::string name{};

	GLFWwindow* window;
	vk::Instance instance{ nullptr };
	vk::DebugUtilsMessengerEXT debugMessenger{ nullptr };
	vk::detail::DispatchLoaderDynamic dldi;
	vk::SurfaceKHR surface;

	//physical device variables
	vk::PhysicalDevice physicalDevice{ nullptr };
	vk::Device device{ nullptr };
	vk::Queue graphicsQueue{ nullptr };
	vk::Queue presentQueue{ nullptr };
	vk::SwapchainKHR swapchain;
	std::vector<vkUtil::SwapchainFrame> swapchainFrames;
	vk::Format swapchainFormat;
	vk::Extent2D swapchainExtent;

	//pipeline variables
	vk::PipelineLayout pipelineLayout;
	vk::RenderPass renderPass;
	vk::Pipeline pipeline;

	//command varibales
	vk::CommandPool commandPool;
	vk::CommandBuffer mainCommandBuffer;

	//synchronization variables
	int maxFramesInFlight, frameNumber;


	void make_instance();

	void make_device();
	void make_swapchain();
	void recreate_swapchain();

	void make_pipeline();

	void finalize_setup();
	void make_framebuffer();
	void make_frame_sync_objects();

	void record_draw_commands(vk::CommandBuffer commandBuffer,Scene* scene, uint32_t imageIndex);

	void cleanup_swapchain();
};