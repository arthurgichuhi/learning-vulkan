#pragma once
#include "./../../EngineConfig.h"

namespace vkInit {
    inline bool supported(std::vector<const char*>& extensions, std::vector<const char*>& layers, bool debug) {
		//check extension support
		std::vector<vk::ExtensionProperties> supportedExtensions = vk::enumerateInstanceExtensionProperties();

		if (debug) {
			std::cout << "Device can support the following extensions:\n";
			for (vk::ExtensionProperties supportedExtension : supportedExtensions) {
				std::cout << '\t' << supportedExtension.extensionName << '\n';
			}
		}

		bool found;
		for (const char* extension : extensions) {
			found = false;
			for (vk::ExtensionProperties supportedExtension : supportedExtensions) {
				if (strcmp(extension, supportedExtension.extensionName) == 0) {
					found = true;
					if (debug) {
						std::cout << "Extension \"" << extension << "\" is supported!\n";
					}
				}
			}
			if (!found) {
				if (debug) {
					std::cout << "Extension \"" << extension << "\" is not supported!\n";
				}
				return false;
			}

			return true;
		}

		//check layer support
		std::vector<vk::LayerProperties> supportedLayers = vk::enumerateInstanceLayerProperties();

		if (debug) {
			std::cout << "Device can support the following layers:\n";
			for (vk::LayerProperties supportedLayer : supportedLayers) {
				std::cout << '\t' << supportedLayer.layerName << '\n';
			}
		}

		for (const char* layer : layers) {
			found = false;
			for (vk::LayerProperties supportedLayer : supportedLayers) {
				if (strcmp(layer, supportedLayer.layerName) == 0) {
					found = true;
					if (debug) {
						std::cout << "Layer \"" << layer << "\" is supported!\n";
					}
				}
			}
			if (!found) {
				if (debug) {
					std::cout << "Layer \"" << layer << "\" is not supported!\n";
				}
				return false;
			}
		}
        return true;
    }

	inline vk::Instance make_instance(bool debug, const char* appName) {

        // Check if we should enable validation layers
        if (debug) {
            std::cout << "Debug mode enabled - validation layers active" << std::endl;
        }

        vk::ApplicationInfo appInfo(
            appName, 1,
            "VulkanEngine", 1,
            VK_API_VERSION_1_0
        );
        //
        uint32_t glfwExtensionsCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

        std::vector<const char*> extensions(glfwExtensions,glfwExtensions+glfwExtensionsCount);

        if (debug) {
            for (const char* extensionName : extensions) {
                std::cout << "\t\"" << extensionName << "\"\"n";
            }
        }

        std::vector<const char*> layers;
        if (debug) {
            extensions.push_back("VK_EXT_debug_utils");
            layers.push_back("VK_LAYER_KHRONOS_validation");

            for (const char* extensionName : extensions) {
                std::cout << "\t\"" << extensionName << "\"\n";
            }
        }
        if (!supported(extensions, layers, debug)) {
            return nullptr;
        }

        // Add validation layers if in debug mode
        vk::InstanceCreateInfo createInfo(
            vk::InstanceCreateFlags(),
            &appInfo,
            static_cast<uint32_t>(layers.size()), layers.data(),//enabled layers
            static_cast<uint32_t>(extensions.size()),extensions.data() //enables extensions
            );

        try {
            return vk::createInstance(createInfo);
        }
        catch (const vk::SystemError& e) {
            std::cerr << "Failed to create Vulkan instance: \n" << e.what() << std::endl;
            return nullptr;
        }
	}
}