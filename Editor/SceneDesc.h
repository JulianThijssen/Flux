#pragma once

#include "MaterialDesc.h"
#include "Skybox.h"
#include "Skysphere.h"

#include "Entity.h"

#include <vector>

namespace Flux {
    namespace Editor {
        class SceneDesc {
        public:
            SceneDesc()
                : skybox(nullptr),
                skysphere(nullptr)
            {

            }

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

            Editor::Skybox* skybox;
            Skysphere* skysphere;
        };
    }
}
