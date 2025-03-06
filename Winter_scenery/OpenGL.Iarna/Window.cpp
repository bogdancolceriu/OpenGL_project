#include "Window.h"

namespace gps {

    void Window::Create(int screenWidth, int screenHeight, const char* windowTitle) {
        if (!glfwInit()) {
            throw std::runtime_error("GLFW initialization failed!");
        }

        // Configure GLFW window hints
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Enable sRGB framebuffer
        glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

        // Enable anti-aliasing with 8 samples
        glfwWindowHint(GLFW_SAMPLES, 8);

        this->window = glfwCreateWindow(screenWidth, screenHeight, windowTitle, nullptr, nullptr);
        if (!this->window) {
            throw std::runtime_error("Failed to create GLFW window!");
        }

        glfwMakeContextCurrent(window);

        glfwSwapInterval(1); // Enable VSync

        // Initialize GLEW for extension handling
        glewExperimental = GL_TRUE;
        GLenum glewStatus = glewInit();
        if (glewStatus != GLEW_OK) {
            throw std::runtime_error("GLEW initialization failed!");
        }

        // Output OpenGL information
        const GLubyte* graphicsRenderer = glGetString(GL_RENDERER);
        const GLubyte* glVersion = glGetString(GL_VERSION);
        std::cout << "Graphics Renderer: " << graphicsRenderer << std::endl;
        std::cout << "OpenGL Version: " << glVersion << std::endl;

        // Adjust for Retina or high-DPI displays
        glfwGetFramebufferSize(window, &this->dimensions.width, &this->dimensions.height);
    }

    void Window::Delete() {
        if (window) {
            glfwDestroyWindow(window);
        }
        // Cleanup GLFW resources
        glfwTerminate();
    }

    GLFWwindow* Window::getWindow() {
        return this->window;
    }

    WindowDimensions Window::getWindowDimensions() {
        return this->dimensions;
    }

    void Window::setWindowDimensions(WindowDimensions newDimensions) {
        this->dimensions = newDimensions;
    }
}
