#pragma once
#ifndef SCENE_H
#define SCENE_H

#include "Entity.h"
#include "Transform.h"
#include "Camera.h"

#include "PointLight.h"

#include <vector>

namespace Flux {
    class Scene {
    public:
        Scene() : mainCamera() { }

        void update() {

        }

        Entity getMainCamera() const {
            return mainCamera;
        }

        void addEntity(Entity* entity) {
            entities.push_back(entity);
        }

        std::vector<Entity*> entities;
        std::vector<Entity*> lights;

        Entity mainCamera;
    };
}

#endif /* SCENE_H */
