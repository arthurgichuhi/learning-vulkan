#pragma once
#include "EngineConfig.h"

namespace vkUtil {

	struct SwapchainFrame {
		vk::Image image;
		vk::ImageView imageView;
		vk::Framebuffer frameBuffer;
		vk::CommandBuffer commandBuffer;
		vk::Semaphore imageAvailable, renderFinished;
		vk::Fence inFlight;
	};
}