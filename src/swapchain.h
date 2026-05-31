#pragma once
#include "EngineConfig.h"
#include "logging.h"
#include "queue_families.h"
#include "frame.h"

namespace vkInit {


	struct SwapChainSupportDetails {
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};

	struct SwapChainBundle {
		vk::SwapchainKHR swapChain;
		std::vector<vkUtil::SwapchainFrame> frames;
		vk::Format format;
		vk::Extent2D extent;
	};

	inline SwapChainSupportDetails get_swap_chain_support(
		vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, bool debug) {

		SwapChainSupportDetails support;
		/*
		* typedef struct VkSurfaceCapabilitiesKHR {
			uint32_t                         minImageCount;
			uint32_t                         maxImageCount;
			VkExtent2D                       currentExtent;
			VkExtent2D                       minImageExtent;
			VkExtent2D                       maxImageExtent;
			uint32_t                         maxImageArrayLayers;
			VkSurfaceTransformFlagsKHR       supportedTransforms;
			VkSurfaceTransformFlagBitsKHR    currentTransform;
			VkCompositeAlphaFlagsKHR         supportedCompositeAlpha;
			VkImageUsageFlags                supportedUsageFlags;
		} VkSurfaceCapabilitiesKHR;
		*/
		support.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);

		if (debug) {

			std::cout << "\nSwap Chain supports the following capabilities";

			std::cout << "\n\tMinimum image count" << support.capabilities.minImageCount;
			std::cout << "\n\tMaximum image count" << support.capabilities.maxImageCount;

		}

		support.formats = physicalDevice.getSurfaceFormatsKHR(surface);

		if (debug) {
			/*
				* typedef struct VkSurfaceFormatKHR {
					VkFormat           format;
					VkColorSpaceKHR    colorSpace;
				} VkSurfaceFormatKHR;
				*/
			for (auto supportedFormat : support.formats) {
				std::cout << "\n supported pixel format" << vk::to_string(supportedFormat.format);
				std::cout << "\n supported color space" << vk::to_string(supportedFormat.colorSpace);
			}
		}

		support.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);

		for (vk::PresentModeKHR presentMode : support.presentModes) {
			std::cout << '\n\t' << log_present_mode(presentMode) << '\n';
		}
		return support;

	}

	inline vk::SurfaceFormatKHR choose_swapchain_surface_format(
		std::vector<vk::SurfaceFormatKHR> formats) {
		for (auto format : formats) {
			if (format.format == vk::Format::eB8G8R8A8Unorm
				&& format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
				return format;
			}
		}

		return formats[0];
	}

	inline vk::PresentModeKHR choose_swapchain_present_mode(
		std::vector<vk::PresentModeKHR> presentModes) {
		for (auto presentMode : presentModes) {
			if (presentMode == vk::PresentModeKHR::eMailbox) {
				return presentMode;
			}
		}

		return vk::PresentModeKHR::eFifo;
	}

	inline vk::Extent2D choose_swapchain_extent(
		uint32_t width, uint32_t height,
		vk::SurfaceCapabilitiesKHR capabilities) {

		if (capabilities.currentExtent.width != UINT32_MAX) {

			return capabilities.currentExtent;
		}
		else {

			vk::Extent2D extent = { width,height };
			extent.width = std::min(
				capabilities.maxImageExtent.width,
				std::max(capabilities.minImageExtent.width, width)
			);

			extent.height = std::min(
				capabilities.maxImageExtent.height,
				std::max(capabilities.maxImageExtent.height, height)
			);

			return extent;
		}
	}


	inline SwapChainBundle create_swapchain(
		vk::Device logicalDevice, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface,
		int width, int height, bool debug) {

		SwapChainSupportDetails support = get_swap_chain_support(physicalDevice, surface, debug);

		auto format = choose_swapchain_surface_format(support.formats);

		vk::PresentModeKHR presentMode = choose_swapchain_present_mode(support.presentModes);

		auto extent = choose_swapchain_extent(width, height, support.capabilities);

		auto imageCount = std::min(
			support.capabilities.maxImageCount,
			support.capabilities.minImageCount + 1
		);

		/*
		* VULKAN_HPP_CONSTEXPR SwapchainCreateInfoKHR(
	  VULKAN_HPP_NAMESPACE::SwapchainCreateFlagsKHR flags_         = {},
	  VULKAN_HPP_NAMESPACE::SurfaceKHR              surface_       = {},
	  uint32_t                                      minImageCount_ = {},
	  VULKAN_HPP_NAMESPACE::Format                  imageFormat_   = VULKAN_HPP_NAMESPACE::Format::eUndefined,
	  VULKAN_HPP_NAMESPACE::ColorSpaceKHR   imageColorSpace_  = VULKAN_HPP_NAMESPACE::ColorSpaceKHR::eSrgbNonlinear,
	  VULKAN_HPP_NAMESPACE::Extent2D        imageExtent_      = {},
	  uint32_t                              imageArrayLayers_ = {},
	  VULKAN_HPP_NAMESPACE::ImageUsageFlags imageUsage_       = {},
	  VULKAN_HPP_NAMESPACE::SharingMode     imageSharingMode_ = VULKAN_HPP_NAMESPACE::SharingMode::eExclusive,
	  uint32_t                              queueFamilyIndexCount_ = {},
	  const uint32_t *                      pQueueFamilyIndices_   = {},
	  VULKAN_HPP_NAMESPACE::SurfaceTransformFlagBitsKHR preTransform_ =
		VULKAN_HPP_NAMESPACE::SurfaceTransformFlagBitsKHR::eIdentity,
	  VULKAN_HPP_NAMESPACE::CompositeAlphaFlagBitsKHR compositeAlpha_ =
		VULKAN_HPP_NAMESPACE::CompositeAlphaFlagBitsKHR::eOpaque,
	  VULKAN_HPP_NAMESPACE::PresentModeKHR presentMode_  = VULKAN_HPP_NAMESPACE::PresentModeKHR::eImmediate,
	  VULKAN_HPP_NAMESPACE::Bool32         clipped_      = {},
	  VULKAN_HPP_NAMESPACE::SwapchainKHR   oldSwapchain_ = {} ) VULKAN_HPP_NOEXCEPT
		*/
		vk::SwapchainCreateInfoKHR createInfo = vk::SwapchainCreateInfoKHR(
			vk::SwapchainCreateFlagsKHR(), surface, imageCount, format.format, format.colorSpace,
			extent, 1, vk::ImageUsageFlagBits::eColorAttachment
		);

		auto indices = vkUtil::findQueueFamilies(physicalDevice, surface, debug);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(),indices.presentFamily.value() };

		if (indices.graphicsFamily.value() != indices.presentFamily.value()) {
			createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = vk::SharingMode::eExclusive;
		}

		createInfo.preTransform = support.capabilities.currentTransform;
		createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

		SwapChainBundle bundle{};
		try {
			bundle.swapChain = logicalDevice.createSwapchainKHR(createInfo);
		}
		catch (vk::SystemError error) {
			throw std::runtime_error("Failed to create swapchain");
		}
		std::vector<vk::Image>images = logicalDevice.getSwapchainImagesKHR(bundle.swapChain);
		bundle.frames.resize(images.size());

		for (size_t i = 0; i < images.size(); ++i) {


			/*
			* ImageViewCreateInfo( VULKAN_HPP_NAMESPACE::ImageViewCreateFlags flags_ = {},
						   VULKAN_HPP_NAMESPACE::Image                image_ = {},
						   VULKAN_HPP_NAMESPACE::ImageViewType    viewType_  = VULKAN_HPP_NAMESPACE::ImageViewType::e1D,
						   VULKAN_HPP_NAMESPACE::Format           format_    = VULKAN_HPP_NAMESPACE::Format::eUndefined,
						   VULKAN_HPP_NAMESPACE::ComponentMapping components_            = {},
						   VULKAN_HPP_NAMESPACE::ImageSubresourceRange subresourceRange_ = {} ) VULKAN_HPP_NOEXCEPT
				: flags( flags_ )
				, image( image_ )
				, viewType( viewType_ )
				, format( format_ )
				, components( components_ )
				, subresourceRange( subresourceRange_ )
			*/

			vk::ImageViewCreateInfo createInfo = {};
			createInfo.image = images[i];
			createInfo.viewType = vk::ImageViewType::e2D;
			createInfo.format = format.format;
			createInfo.components.r = vk::ComponentSwizzle::eIdentity;
			createInfo.components.g = vk::ComponentSwizzle::eIdentity;
			createInfo.components.b = vk::ComponentSwizzle::eIdentity;
			createInfo.components.a = vk::ComponentSwizzle::eIdentity;
			createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			bundle.frames[i].image = images[i];
			bundle.frames[i].imageView = logicalDevice.createImageView(createInfo);
		}

		bundle.format = format.format;
		bundle.extent = extent;

		return bundle;

	}
}