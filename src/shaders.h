#include "EngineConfig.h"

namespace vkUtil {
	std::vector<char> readFile(std::string filename, bool debug) {

		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if(debug && !file.is_open()){
			std::cout << "\nUtil: Failed to load " << filename << std::endl;
		}

		size_t filesize(static_cast<size_t>(file.tellg()));

		std::vector<char> buffer(filesize);
		file.seekg(0);
		file.read(buffer.data(), filesize);

		file.close();

		return buffer;
	}

	vk::ShaderModule createModule(std::string filename,vk::Device device,bool debug){
	
		std::vector<char> sourceCode = readFile(filename,debug);
		vk::ShaderModuleCreateInfo moduleInfo = {};
		moduleInfo.flags = vk::ShaderModuleCreateFlags();
		moduleInfo.codeSize = sourceCode.size();
		moduleInfo.pCode = reinterpret_cast<uint32_t*>(sourceCode.data());
		std::cout << sourceCode.data() << std::endl;

		try {

			return device.createShaderModule(moduleInfo);
		}
		catch (vk::SystemError error) {
		
			std::cout << "\n Failed to create shader module \"" << filename << "\"" << std::endl;
		}
	}
}