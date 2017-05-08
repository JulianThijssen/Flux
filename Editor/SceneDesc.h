#pragma once
#ifndef SCENE_DESC_H
#define SCENE_DESC_H

#include "MaterialDesc.h"
#include "Skybox.h"
#include "Skysphere.h"

#include <Engine/Source/Entity.h>
#include <Engine/Source/Transform.h>
#include <Engine/Source/Camera.h>
#include <Engine/Source/PointLight.h>

#include <vector>

namespace Flux {
    class SceneDesc {
    public:
        void addMaterial(MaterialDesc* materialDesc) {
            materials.push_back(materialDesc);
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
            return nullptr;
        }

        std::vector<MaterialDesc*> materials;
        std::vector<Entity*> entities;

        Skybox* skybox;
        Skysphere* skysphere;
    };
}

#endif /* SCENE_DESC_H */
