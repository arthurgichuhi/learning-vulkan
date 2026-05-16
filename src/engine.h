#pragma once
#include <GLFW/glfw3.h>
#include "config.h"

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

		void build_glfw_window();

		void make_instance();
	};
}