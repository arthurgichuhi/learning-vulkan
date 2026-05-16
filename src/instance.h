#pragma once
#include "config.h"

namespace vkInit {
	vk::Instance make_instance(bool debug, const char* appName) {

		uint32_t version{ 0 };

		vkEnumerateInstanceVersion(&version);
	
		return nullptr;
	}
}