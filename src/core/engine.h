#pragma once
#include <GLFW/glfw3.h>
#include "../EngineConfig.h"
#include "../EngineInstance.h"
#include "../logging.h"
#include "../device.h"

namespace engine {
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

		vk::PhysicalDevice device{ nullptr };

		void build_glfw_window();

		void make_instance();

		void make_device();
	};
}