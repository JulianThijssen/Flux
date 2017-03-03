#include "Input.h"

#include <iostream>

namespace Flux {
    Vector2f Input::mousePos(0, 0);
    Vector2f Input::deltaMousePos(0, 0);

    bool Input::keys[400];

    void Input::addKeyEvent(const int key, const bool state) {
        keys[key] = state;
    }

    bool Input::isKeyDown(const int key) {
        return keys[key];
    }

    void Input::addMouseMoveEvent(float x, float y) {
        deltaMousePos.set(x - mousePos.x, y - mousePos.y);
        mousePos.set(x, y);
    }

    Vector2f Input::getMousePos() {
        return mousePos;
    }
}
