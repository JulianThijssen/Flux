#pragma once
#ifndef SCENE_H
#define SCENE_H

#include "Entity.h"
#include "Transform.h"
#include "Camera.h"

namespace Flux {
    class Scene {
    public:
        Scene() : mainCamera()
        {
            mainCamera.addComponent(new Transform());
            mainCamera.addComponent(new Camera(60, 1, 0.1f, 100.f));
        }

        void update() {

        }

        Entity getMainCamera() const {
            return mainCamera;
        }
    private:
        Entity mainCamera;
    };
}

#endif /* SCENE_H */
