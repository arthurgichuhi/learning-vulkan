#pragma once
#include <GLFW/glfw3.h>
#include "../EngineConfig.h"
#include "../EngineInstance.h"
#include "../logging.h"

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

		void build_glfw_window();

		void make_instance();

		void make_debug_messenger();
	};
}