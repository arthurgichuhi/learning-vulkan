#pragma once
#include "EngineConfig.h"
#include "./core/engine.h"
#include "./core/scene.h"

class App {
private:
	Engine* graphicsEngine;
	GLFWwindow* window;
	Scene* scene;

	double lastTime, currentTime;
	int numFrames;
	float frameTime;

	void build_glfw_window(int width, int height, bool debugMode);

	void calculate_frame_rate();

public:
	App(int width, int height, bool debug);
	~App();

	void run();
};