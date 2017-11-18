/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** Window.h
** Declares a window class containing basic window creation functionality
** and initialization of an OpenGL context through GLFW.
**
** Author: Julian Thijssen
** -------------------------------------------------------------------------*/

#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Flux {
    class Window {
    public:
        Window();
        Window(const char* title);
        Window(const char* title, int width, int height);

        bool create();
        std::string getTitle();
        const unsigned int getWidth();
        const unsigned int getHeight();
        void setTitle(std::string title);
        void setSize(int width, int height);
        void update();
        void close();
        bool isClosed();
    private:
        GLFWwindow* window;
        std::string title;
        unsigned int width;
        unsigned int height;

        void destroy();
        static void onError(int error, const char* description);
        static void onKeyAction(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void onMouseMove(GLFWwindow* window, double x, double y);
    };
}

#endif /* WINDOW_H */
