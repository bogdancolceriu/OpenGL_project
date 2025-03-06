#ifndef WINDOW_H
#define WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <iostream>

struct WindowProperties {
    int width;
    int height;
};

namespace gps {

    class RenderWindow {

    public:
        void Initialize(int screenWidth = 1024, int screenHeight = 768, const char* windowTitle = "Rendering Application");
        void Destroy();

        GLFWwindow* retrieveWindowHandle();
        WindowProperties getDimensions();
        void updateDimensions(WindowProperties newDimensions);

    private:
        WindowProperties properties;
        GLFWwindow* glfwWindowHandle;
    };
}

#endif //WINDOW_H
