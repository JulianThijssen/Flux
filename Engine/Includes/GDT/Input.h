#pragma once

namespace GDT
{
    class KeyListener
    {
    public:
        virtual void onKeyPressed(int key, int mods) = 0;
        virtual void onKeyReleased(int key, int mods) = 0;
    };

    class MouseMoveListener
    {
    public:
        virtual void onMouseMove(float x, float y) = 0;
    };

    class MouseClickListener
    {
    public:
        virtual void onMouseClicked(int button, int mods) = 0;
        virtual void onMouseReleased(int button, int mods) = 0;
    };
}
