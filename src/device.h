#pragma once
#include "EngineConfig.h"
#include "queue_families.h"


/*
* Vulkan separates the concept of physical and logical devices.
*
  A physical device usually represents a single complete implementation of Vulkan
  (excluding instance-level functionality) available to the host,
  of which there are a finite number.

  A logical device represents an instance of that implementation
  with its own state and resources independent of other logical devices.
*/


namespace vkInit {

	inline void log_device_properties(const vk::PhysicalDevice& device) {
		/*
		* void vkGetPhysicalDeviceProperties(
			VkPhysicalDevice                            physicalDevice,
			VkPhysicalDeviceProperties*                 pProperties);
		*/

		vk::PhysicalDeviceProperties properties = device.getProperties();

		/*
		* typedef struct VkPhysicalDeviceProperties {
			uint32_t                            apiVersion;
			uint32_t                            driverVersion;
			uint32_t                            vendorID;
			uint32_t                            deviceID;
			VkPhysicalDeviceType                deviceType;
			char                                deviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE];
			uint8_t                             pipelineCacheUUID[VK_UUID_SIZE];
			VkPhysicalDeviceLimits              limits;
			VkPhysicalDeviceSparseProperties    sparseProperties;
			} VkPhysicalDeviceProperties;
		*/

		std::cout << "Device name: " << properties.deviceName << '\n';

		std::cout << "Device type: ";
		switch (properties.deviceType) {

		case (vk::PhysicalDeviceType::eCpu):
			std::cout << "CPU\n";
			break;

		case (vk::PhysicalDeviceType::eDiscreteGpu):
			std::cout << "Discrete GPU\n";
			break;

		case (vk::PhysicalDeviceType::eIntegratedGpu):
			std::cout << "Integrated GPU\n";
			break;

		case (vk::PhysicalDeviceType::eVirtualGpu):
			std::cout << "Virtual GPU\n";
			break;

		default:
			std::cout << "Other\n";
		}
	}

	/**
		Check whether the physical device can support the given extensions.

		\param device the physical device to check
		\param requestedExtensions a list of extension names to check against
		\param debug whether the system is running in debug mode
		\returns whether all of the extensions are requested
	*/
	inline bool checkDeviceExtensionSupport(
		const vk::PhysicalDevice& device,
		const std::vector<const char*>& requestedExtensions,
		const bool& debug
	) {

		/*
		* Check if a given physical device can satisfy a list of requested device
		* extensions.
		*/
		
		std::set<std::string> requiredExtensions(requestedExtensions.begin(), requestedExtensions.end());

		if (debug) {
			std::cout << "Device can support extensions:\n";
		}

		for (vk::ExtensionProperties& extension : device.enumerateDeviceExtensionProperties()) {

			if (debug) {
				std::cout << "\t\"" << extension.extensionName << "\"\n";
			}

			//remove this from the list of required extensions (set checks for equality automatically)
			requiredExtensions.erase(extension.extensionName);
		}

		//if the set is empty then all requirements have been satisfied
		return requiredExtensions.empty();
	}


	/**
		Check whether the given physical device is suitable for the system.

		\param device the physical device to check.
		\debug whether the system is running in debug mode.
		\returns whether the device is suitable.
	*/
	inline bool isSuitable(const vk::PhysicalDevice& device, const bool debug) {

		if (debug) {
			std::cout << "Checking if device is suitable\n";
		}

		/*
		* A device is suitable if it can present to the screen, ie support
		* the swapchain extension
		*/
		const std::vector<const char*> requestedExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		if (debug) {
			std::cout << "We are requesting device extensions:\n";

			for (const char* extension : requestedExtensions) {
				std::cout << "\t\"" << extension << "\"\n";
			}

		}

		if (bool extensionsSupported = checkDeviceExtensionSupport(device, requestedExtensions, debug)) {

			if (debug) {
				std::cout << "Device can support the requested extensions!\n";
			}
		}
		else {

			if (debug) {
				std::cout << "Device can't support the requested extensions!\n";
			}

			return false;
		}
		return true;
	}


	inline vk::PhysicalDevice choose_physical_device(vk::Instance& instance, const bool debug) {
		/*
		* Choose a suitable physical device from a list of candidates.
		* Note: Physical devices are neither created nor destroyed, they exist
		* independently to the program.
		*/

		if (debug) {
			std::cout << "Choosing Physical Device\n";
		}

		/*
		* ResultValueType<std::vector<PhysicalDevice, PhysicalDeviceAllocator>>::type
			Instance::enumeratePhysicalDevices( Dispatch const & d )

		  std::vector<vk::PhysicalDevice> instance.enumeratePhysicalDevices( Dispatch const & d = static/default )
		*/
		std::vector<vk::PhysicalDevice> availableDevices = instance.enumeratePhysicalDevices();

		if (debug) {
			std::cout << "There are " << availableDevices.size() << " physical devices available on this system\n";
		}

		/*
		* check if a suitable device can be found
		*/
		for (vk::PhysicalDevice device : availableDevices) {

			if (debug) {
				log_device_properties(device);
			}

			return device;
			if (isSuitable(device, debug)) {
				return device;
			}
		}

		return nullptr;

	}



	inline vk::Device create_logical_device(
		vk::PhysicalDevice& physicalDevice,vk::SurfaceKHR surface, bool& debug) {

		vkUtil::QueueFamilyIndices indices = vkUtil::findQueueFamilies(physicalDevice, surface, debug);
		std::vector<uint32_t>uniqueIndices;
		uniqueIndices.push_back(indices.graphicsFamily.value());
		if (indices.graphicsFamily.value() != indices.presentFamily.value()) {
			uniqueIndices.push_back(indices.presentFamily.value());
		}

		float queuePriority = 1.0f;
		/*
		* VULKAN_HPP_CONSTEXPR DeviceQueueCreateInfo( VULKAN_HPP_NAMESPACE::DeviceQueueCreateFlags flags_            = {},
												uint32_t                                     queueFamilyIndex_ = {},
												uint32_t                                     queueCount_       = {},
												const float * pQueuePriorities_ = {} ) VULKAN_HPP_NOEXCEPT
		*/
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo;

		for (uint32_t queueFamilyIndex : uniqueIndices) {
			queueCreateInfo.push_back(vk::DeviceQueueCreateInfo(
				vk::DeviceQueueCreateFlags(), indices.graphicsFamily.value(),
				1, &queuePriority
			));
		}

		std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		/*
		* Device features must be requested before the device is abstracted,
		* therefore we only pay for what we need.
		*/

		vk::PhysicalDeviceFeatures deviceFeatures = vk::PhysicalDeviceFeatures();

		/*
		* VULKAN_HPP_CONSTEXPR DeviceCreateInfo( VULKAN_HPP_NAMESPACE::DeviceCreateFlags flags_                         = {},
										   uint32_t                                queueCreateInfoCount_          = {},
										   const VULKAN_HPP_NAMESPACE::DeviceQueueCreateInfo * pQueueCreateInfos_ = {},
										   uint32_t                                            enabledLayerCount_ = {},
										   const char * const * ppEnabledLayerNames_                              = {},
										   uint32_t             enabledExtensionCount_                            = {},
										   const char * const * ppEnabledExtensionNames_                          = {},
										   const VULKAN_HPP_NAMESPACE::PhysicalDeviceFeatures * pEnabledFeatures_ = {} )
		*/

		std::vector<char*> enabledLayers;

		if (debug) {
			enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
		}

		vk::DeviceCreateInfo deviceCreateInfo = vk::DeviceCreateInfo(
			vk::DeviceCreateFlags(), queueCreateInfo.size(), queueCreateInfo.data(),
			enabledLayers.size(), enabledLayers.data(),//layers
			deviceExtensions.size(), deviceExtensions.data(),//extensions
			&deviceFeatures
		);

		try {

			if (debug) {
				std::cout << "\n" << "Device Succesfully Abstracted" << "\n";
			}
			return physicalDevice.createDevice(deviceCreateInfo);
		}
		catch (vk::SystemError error) {
			if (debug) {
				std::cout << "\n" << "Device Abstraction Failed" << "\n";
				return nullptr;
			}
		}
	}

	inline std::array< vk::Queue,2> get_queue(
		vk::PhysicalDevice physicalDevice,vk::Device device, vk::SurfaceKHR surface, bool debug) {
		vkUtil::QueueFamilyIndices indices = vkUtil::findQueueFamilies(physicalDevice, surface, debug);

		return {
			device.getQueue(indices.graphicsFamily.value(), 0),
			device.getQueue(indices.presentFamily.value(), 0)
		};
	}

	
}