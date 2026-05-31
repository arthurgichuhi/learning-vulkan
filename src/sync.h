#pragma once
#include "EngineConfig.h"

namespace vkInit {
	vk::Semaphore make_semaphore(vk::Device device,bool debug){
		vk::SemaphoreCreateInfo sempahoreInfo = {};
		sempahoreInfo.flags = vk::SemaphoreCreateFlags();

		try {
			return device.createSemaphore(sempahoreInfo);
		}
		catch (vk::SystemError error) {
			if (debug) {
				std::cout << "Failed to create semaphore" << std::endl;
			}
			return nullptr;
		}
	}

	vk::Fence make_fence(vk::Device device, bool debug) {

		vk::FenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.flags = vk::FenceCreateFlags() | vk::FenceCreateFlagBits::eSignaled;

		try { return device.createFence(fenceCreateInfo); }
		catch (vk::SystemError error) {
			if (debug) {
				std::cout << "Failed to create fence" << std::endl;
			 }

			return nullptr;
		}

	}
}