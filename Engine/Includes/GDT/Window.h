#pragma once

#include "Input.h"

#include <vector>
#include <string>
#include <exception>

typedef unsigned int uint;

struct GLFWwindow;

namespace GDT
{
    class WindowCreationException : public std::exception
    {
    public:
        WindowCreationException(std::string errorMessage)
            : errorMessage(errorMessage)
        { }

        const char *what() const noexcept override
        {
            return errorMessage.c_str();
        }

    private:
        std::string errorMessage;
    };

    struct GlSettings
    {
        uint majorVersion;
        uint minorVersion;
        bool coreProfile;
    };

    class Window
    {
    public:
        uint getWidth() const;
        uint getHeight() const;

        void create(std::string title, uint width, uint height);
        void pollEvents();
        void swapBuffers();
        void update();
        bool shouldClose();
        void close();
        void destroy();

        void setGlVersion(uint majorVersion, uint minorVersion, bool coreProfile);
        void setResizable(bool resizable);
        void lockCursor(bool lock);
        void enableVSync(bool enable);

        void addKeyListener(KeyListener* keyListener);
        void addMouseMoveListener(MouseMoveListener* mouseMoveListener);
        void addMouseClickListener(MouseClickListener* mouseClickListener);

        void onKeyInput(int key, int action, int mods);
        void onMouseMove(float x, float y);
        void onMouseButton(int button, int action, int mods);

    private:
        GLFWwindow* window;

        uint _width, _height;

        GlSettings _glSettings = { 3, 3, true };

        // Non-owning lists of input listener pointer references
        std::vector<KeyListener*> keyListeners;
        std::vector<MouseMoveListener*> mouseMoveListeners;
        std::vector<MouseClickListener*> mouseClickListeners;
    };
}
