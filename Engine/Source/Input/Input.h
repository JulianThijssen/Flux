#pragma once
#ifndef INPUT_H
#define INPUT_H

#include "Vector2f.h"

namespace Flux {
    class Input {
    public:
        static const int KEY_W = 87;
        static const int KEY_S = 83;
        static const int KEY_A = 65;
        static const int KEY_D = 68;

        static void addKeyEvent(const int key, const bool state);
        static bool isKeyDown(const int key);

        static void addMouseMoveEvent(float x, float y);
        static Vector2f getMousePos();
    private:
        static bool keys[];

        static Vector2f mousePos;
        static Vector2f deltaMousePos;
    };
}

#endif /* INPUT_H */
