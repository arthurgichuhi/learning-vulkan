#pragma once
#include "EngineConfig.h"
#include "frame.h"

namespace vkInit{
	struct FrameBufferInput {
		vk::Device device;
		vk::RenderPass renderpass;
		vk::Extent2D swapchainExtent;
	};

	void make_framebuffers(
		FrameBufferInput inputChunk,std::vector<vkUtil::SwapchainFrame>& frames, bool debug){
		for (size_t i = 0; i < frames.size(); ++i) {

			std::vector<vk::ImageView> attachments = {
					frames[i].imageView
			};

			vk::FramebufferCreateInfo framebufferInfo;
			framebufferInfo.flags = vk::FramebufferCreateFlags();
			framebufferInfo.renderPass = inputChunk.renderpass;
			framebufferInfo.attachmentCount = attachments.size();
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = inputChunk.swapchainExtent.width;
			framebufferInfo.height = inputChunk.swapchainExtent.height;
			framebufferInfo.layers = 1;

			try {
				frames[i].frameBuffer = inputChunk.device.createFramebuffer(framebufferInfo);

				if (debug) {
					std::cout << "Created framebuffer for frame " << i << std::endl;
				}
			}
			catch (vk::SystemError err) {
				if (debug) {
					std::cout << "Failed to create framebuffer for frame " << i << std::endl;
				}
			}
		}
		
	}
}