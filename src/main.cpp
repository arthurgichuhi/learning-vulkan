#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <iostream>

int main() {
    std::cout << "Hello Vulkan with GLFW!" << std::endl;

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    std::cout << "GLFW initialized successfully!" << std::endl;
    
    // Cleanup
    glfwTerminate();
    
    return 0;
}