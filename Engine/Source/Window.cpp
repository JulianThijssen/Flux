/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** Window.cpp
** Implements a window class containing basic window creation functionality
** and initialization of an OpenGL context through GLFW.
**
** Author: Julian Thijssen
** -------------------------------------------------------------------------*/

#include "Window.h"

#include "Log.h"
#include "Input/Input.h"
#include <iostream>

namespace {
    const static char* DEFAULT_TITLE = "Window";
    const static int DEFAULT_WIDTH = 800;
    const static int DEFAULT_HEIGHT = 600;
}

namespace Flux {
    Window::Window() :
        Window(DEFAULT_TITLE) {
    }

    Window::Window(const char* title) :
        Window(title, DEFAULT_WIDTH, DEFAULT_HEIGHT) {
    }

    Window::Window(const char* title, int width, int height) :
        title(title), width(width), height(height) {
        glfwSetErrorCallback(onError);
        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(width, height, title, NULL, NULL);
        glfwMakeContextCurrent(window);
        glfwSwapInterval(0);

        glfwSetKeyCallback(window, onKeyAction);
        glfwSetCursorPosCallback(window, onMouseMove);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (!gladLoadGL())
        {
            Log::error("Failed to initialize OpenGL context");
        }
    }

    std::string Window::getTitle() {
        return title;
    }

    const unsigned int Window::getWidth() {
        return width;
    }

    const unsigned int Window::getHeight() {
        return height;
    }

    void Window::setTitle(std::string title) {
        this->title = title;
        glfwSetWindowTitle(window, title.c_str());
    }

    void Window::setSize(int width, int height) {
        glfwSetWindowSize(window, width, height);
    }

    void Window::update() {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    void Window::close() {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    void Window::destroy() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    bool Window::isClosed() {
        if (glfwWindowShouldClose(window) == GL_FALSE) {
            return false;
        }
        return true;
    }

    void Window::onKeyAction(GLFWwindow* window, int key, int scancode, int action, int mods) {
        Input::addKeyEvent(key, action == GLFW_PRESS || action == GLFW_REPEAT);
    }

    void Window::onMouseMove(GLFWwindow* window, double x, double y) {
        Input::addMouseMoveEvent((float) x, (float) y);
    }

    void Window::onError(int error, const char* description) {
        switch (error) {
        case GLFW_NOT_INITIALIZED:
            Log::error("A window function was called while the window was not initialized");
        case GLFW_NO_CURRENT_CONTEXT:
            Log::error("A window function was called that depends on OpenGL, but no GL context is set");
        case GLFW_INVALID_ENUM:
            Log::error("A window function was called with an invalid argument enum");
        case GLFW_INVALID_VALUE:
            Log::error("A window function was called with an invalud argument value");
        case GLFW_OUT_OF_MEMORY:
            Log::error("A window function caused a failed memory allocation");
        case GLFW_API_UNAVAILABLE:
            Log::error("The window could not find support for the requested API");
        case GLFW_VERSION_UNAVAILABLE:
            Log::error("The requested OpenGL version is not available on this machine");
        case GLFW_PLATFORM_ERROR:
            Log::error("A window function caused a platform error");
        case GLFW_FORMAT_UNAVAILABLE:
            Log::error("The requested pixel format is not supported or the contents of the clipboard could not be converted to the requested format");
        }
    }
}
