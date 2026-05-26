#include "EngineConfig.h"

namespace vkUtil {
	std::vector<char> readFile(std::string filename, bool debug) {

		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if(debug && !file.is_open()){
			std::cout << "\n Failed to load" << filename << "\n";
		}

		size_t filesize(static_cast<size_t>(file.tellg()));

		std::vector<char> buffer(filesize);
		file.seekg(0);
		file.read(buffer.data(), filesize);

		file.close();
	}
}