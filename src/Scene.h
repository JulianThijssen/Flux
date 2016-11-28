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
        Scene() : mainCamera()
        {
            mainCamera.addComponent(new Transform());
            mainCamera.addComponent(new Camera(60, 1, 0.1f, 100.f));

            Entity* light = new Entity();
            PointLight* point = new PointLight();
            light->addComponent(point);
            Transform* transform = new Transform();
            transform->position.set(5, 5, 5);
            light->addComponent(transform);
            lights.push_back(light);
        }

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

    private:
        Entity mainCamera;
    };
}

#endif /* SCENE_H */
