#include "engine.h"
#include "../logging.h"
#include "../device.h"
#include "../swapchain.h"
#include "../pipeline.h"
#include "../framebuffer.h"
#include "../commands.h"
#include "../sync.h"


Engine::Engine(int width, int height, GLFWwindow* window, bool debug) {
	this->width = width;
	this->height = height;
	this->window = window;
	debugMode = debug;
	make_instance();
	make_device();
	make_pipeline();
	finalize_setup();
}

Engine::~Engine() {

	device.waitIdle();

	device.destroyFence(inFlightFence);
	device.destroySemaphore(imageAvailable);
	device.destroySemaphore(renderFinished);

	device.destroyCommandPool(commandPool);

	device.destroyPipeline(pipeline);
	device.destroyPipelineLayout(layout);
	device.destroyRenderPass(renderPass);

	for (vkUtil::SwapchainFrame frame : swapchainFrames) {
		device.destroyImageView(frame.imageView);
		device.destroyFramebuffer(frame.frameBuffer);
	}
	device.destroySwapchainKHR(swapchain);
	device.destroy();
	instance.destroySurfaceKHR(surface);
	instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldi);
	instance.destroy();
	glfwDestroyWindow(window);
	glfwTerminate();
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

void Engine::finalize_setup(){
	vkInit::FrameBufferInput frameBufferInput;
	frameBufferInput.device = device;
	frameBufferInput.renderpass = renderPass;
	frameBufferInput.swapchainExtent = swapchainExtent;
	vkInit::make_framebuffers(frameBufferInput, swapchainFrames, debugMode);

	commandPool = vkInit::make_command_pool(device, physicalDevice, surface, debugMode);

	vkInit::commandBufferInputChunk commandBufferInput = { device, commandPool, swapchainFrames };
	mainCommandBuffer = vkInit::make_command_buffers(commandBufferInput, debugMode);

	inFlightFence = vkInit::make_fence(device, debugMode);
	imageAvailable = vkInit::make_semaphore(device, debugMode);
	renderFinished = vkInit::make_semaphore(device, debugMode);
}

void Engine::record_draw_commands(vk::CommandBuffer commandBuffer, uint32_t imageIndex) {

	vk::CommandBufferBeginInfo beginInfo = {};
	try {
		commandBuffer.begin(beginInfo);
	}
	catch (vk::SystemError error) {

		if(debugMode){
			std::cout << "Failed to begin command buffer" << std::endl;
		}
	}

	vk::RenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapchainFrames[imageIndex].frameBuffer;
	renderPassInfo.renderArea.offset.x = 0;
	renderPassInfo.renderArea.offset.y = 0;
	renderPassInfo.renderArea.extent = swapchainExtent;
	vk::ClearValue clearColor = { std::array<float,4>{1.0f, 0.5f, 0.25f, 1.0f} };
	renderPassInfo.pClearValues = &clearColor;

	commandBuffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);

	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

	commandBuffer.draw(3,1,0,0);

	commandBuffer.endRenderPass();

	try {

		commandBuffer.end();
	}
	catch (vk::SystemError error) {
		if(debugMode){
			std::cout << "Failed to finishing recording command buffer" << std::endl;
		}
	}
}

void Engine::render() {

	device.waitForFences(1, &inFlightFence, VK_TRUE, UINT64_MAX);
	device.resetFences(1, &inFlightFence);

	uint32_t imageIndex{ device.acquireNextImageKHR(swapchain,UINT64_MAX,imageAvailable,nullptr).value };
	
	auto commandBuffer = swapchainFrames[imageIndex].commandBuffer;

	commandBuffer.reset();

	record_draw_commands(commandBuffer, imageIndex);

	vk::SubmitInfo submitInfo = {};
	vk::Semaphore waitSemaphores[] = { imageAvailable };
	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	vk::Semaphore signalSemaphores[] = { renderFinished };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	try {

		graphicsQueue.submit(submitInfo, inFlightFence);
	}
	catch (vk::SystemError error) {
		if (debugMode) {
			std::cout << "Failed to submit draw command buffer" << std::endl;
		}
	}

	vk::PresentInfoKHR presentInfo = {};
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	vk::SwapchainKHR swapchains[] = { swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;

	presentQueue.presentKHR(presentInfo);
}