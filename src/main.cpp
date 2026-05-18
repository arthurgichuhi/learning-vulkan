#include <GLFW/glfw3.h>
#include <iostream>
#include "core/engine.h"

int main() {
    engine::Engine* engine = new engine::Engine();

    delete engine;
    
    return 0;
}