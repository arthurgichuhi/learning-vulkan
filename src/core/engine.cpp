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
	make_assets();
}

Engine::~Engine() {

	device.waitIdle();

	device.destroyCommandPool(commandPool);

	device.destroyPipeline(pipeline);
	device.destroyPipelineLayout(pipelineLayout);
	device.destroyRenderPass(renderPass);

	cleanup_swapchain();

	delete triangleMesh;

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

	make_swapchain();
	
	frameNumber = 0;
}

void Engine::make_swapchain(){
	auto bundle = vkInit::create_swapchain(device, physicalDevice, surface, width, height, debugMode);
	swapchain = bundle.swapChain;
	swapchainFrames = bundle.frames;
	swapchainFormat = bundle.format;
	swapchainExtent = bundle.extent;
	maxFramesInFlight = static_cast<int>(swapchainFrames.size());
}

void Engine::recreate_swapchain() {

	width = 0;
	height = 0;
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	device.waitIdle();

	cleanup_swapchain();
	make_swapchain();
	make_framebuffer();
	make_frame_sync_objects();
	vkInit::commandBufferInputChunk commandBufferInput = {device,commandPool,swapchainFrames};
	vkInit::make_frame_command_buffer(commandBufferInput, debugMode);
}

void Engine::make_pipeline() {
	vkInit::GraphicsPipelineInBundle specification = {};

	specification.device = device;
	specification.vertexFilepath = "./../../../src/shaders/vertex.spv";
	specification.fragmentFilePath = "./../../../src/shaders/fragment.spv";
	specification.swapchainExtent = swapchainExtent;
	specification.swapchainImageFormat = swapchainFormat;

	auto output = vkInit::make_graphics_pipeline(specification, debugMode);
	pipelineLayout = output.layout;
	renderPass = output.renderPass;
	pipeline = output.pipeline;

}

void Engine::make_framebuffer() {
	vkInit::FrameBufferInput frameBufferInput;
	frameBufferInput.device = device;
	frameBufferInput.renderpass = renderPass;
	frameBufferInput.swapchainExtent = swapchainExtent;
	vkInit::make_framebuffers(frameBufferInput, swapchainFrames, debugMode);
}

void Engine::make_frame_sync_objects() {
	for (auto& frame : swapchainFrames) {
		frame.inFlight = vkInit::make_fence(device, debugMode);
		frame.imageAvailable = vkInit::make_semaphore(device, debugMode);
		frame.renderFinished = vkInit::make_semaphore(device, debugMode);
	}
}

void Engine::finalize_setup(){

	make_framebuffer();
	
	commandPool = vkInit::make_command_pool(device, physicalDevice, surface, debugMode);

	vkInit::commandBufferInputChunk commandBufferInput = { device, commandPool, swapchainFrames };
	mainCommandBuffer = vkInit::make_command_buffer(commandBufferInput, debugMode);
	vkInit::make_frame_command_buffer(commandBufferInput, debugMode);

	make_frame_sync_objects();
}

void Engine::make_assets() {
	triangleMesh = new TriangleMesh(device,physicalDevice);
}

void Engine::prepare_scene(vk::CommandBuffer commandBuffer) {
	vk::Buffer vertexBuffers[] = { triangleMesh->vertexBuffer.buffer };
	vk::DeviceSize offsets[] = { 0 };
	commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
}

void Engine::record_draw_commands(vk::CommandBuffer commandBuffer,Scene* scene, uint32_t imageIndex) {

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
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	commandBuffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);

	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
	prepare_scene(commandBuffer);
	for (auto position : scene->trianglePositions) {
		glm::mat4 model = glm::translate(glm::mat4(1.0), position);
		vkUtil::ObjectData objData;
		objData.model = model;
		commandBuffer.pushConstants(
			pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0,sizeof(objData), & objData);
		commandBuffer.draw(3, 1, 0, 0);
	}


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

void Engine::render(Scene* scene) {

	device.waitForFences(1, &swapchainFrames[frameNumber].inFlight, VK_TRUE, UINT64_MAX);
	device.resetFences(1, &swapchainFrames[frameNumber].inFlight);

	uint32_t imageIndex;
	try {
		vk::ResultValue acquire = device.acquireNextImageKHR(
			swapchain, UINT64_MAX,
			swapchainFrames[frameNumber].imageAvailable, nullptr
		);
		imageIndex = acquire.value;
	}
	catch (vk::OutOfDateKHRError error) {
		std::cout << "Recreate" << std::endl;
		recreate_swapchain();
		return;
	}
	catch (vk::IncompatibleDisplayKHRError error) {
		std::cout << "Recreate" << std::endl;
		recreate_swapchain();
		return;
	}
	catch (vk::SystemError error) {
		std::cout << "Failed to acquire swapchain image!" << std::endl;
	}
	
	auto commandBuffer = swapchainFrames[frameNumber].commandBuffer;

	commandBuffer.reset();

	record_draw_commands(commandBuffer,scene, imageIndex);

	vk::SubmitInfo submitInfo = {};
	vk::Semaphore waitSemaphores[] = { swapchainFrames[frameNumber].imageAvailable };
	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	vk::Semaphore signalSemaphores[] = { swapchainFrames[frameNumber].renderFinished };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	try {

		graphicsQueue.submit(submitInfo, swapchainFrames[frameNumber].inFlight);
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

	vk::Result present;

	try {
		present = presentQueue.presentKHR(presentInfo);
	}
	catch (vk::OutOfDateKHRError error) {
		present = vk::Result::eErrorOutOfDateKHR;
	}

	if (present == vk::Result::eErrorOutOfDateKHR || present == vk::Result::eSuboptimalKHR) {
		std::cout << "Recreate" << std::endl;
		recreate_swapchain();
		return;
	}

	frameNumber = (frameNumber + 1) % maxFramesInFlight;
}

void Engine::cleanup_swapchain() {
	for (vkUtil::SwapchainFrame frame : swapchainFrames) {
		device.destroyImageView(frame.imageView);
		device.destroyFramebuffer(frame.frameBuffer);
		device.destroyFence(frame.inFlight);
		device.destroySemaphore(frame.imageAvailable);
		device.destroySemaphore(frame.renderFinished);
	}
	device.destroySwapchainKHR(swapchain);
}