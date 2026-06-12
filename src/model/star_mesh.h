#pragma once
#include "../EngineConfig.h"
#include "../core/vkUtil/memory.h"

class StarMesh {
public:
	StarMesh(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice);
	~StarMesh();
	Buffer vertexBuffer;
private:
	vk::Device logicalDevice;
};