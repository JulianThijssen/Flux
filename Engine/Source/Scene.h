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

        Entity* getMainCamera() const {
            return mainCamera;
        }

        void addEntity(Entity* entity) {
            entities.push_back(entity);
        }

        Entity* getEntityById(uint32_t id) const {
            for (Entity* e : entities) {
                if (e->getId() == id) {
                    return e;
                }
            }
            for (Entity* e : entities) {
                if (e->getId() == id) {
                    return e;
                }
            }
            if (mainCamera->getId() == id) {
                return mainCamera;
            }
            return nullptr;
        }

        std::vector<Entity*> entities;
        std::vector<Entity*> lights;

        Entity* mainCamera;
    };
}

#endif /* SCENE_H */
