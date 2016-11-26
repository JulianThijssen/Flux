#pragma once
#ifndef SCENE_H
#define SCENE_H

#include "Entity.h"
#include "Transform.h"

namespace Flux {
    class Scene {
    public:
        Scene() : mainCamera()
        {
            mainCamera.addComponent(Transform());
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
