#pragma once
#ifndef ENTITY_H
#define ENTITY_H

#include "Component.h"
#include "Exceptions/ComponentNotFoundException.h"

#include <vector>

namespace Flux {
    static uint32_t idGenerator = 0;

    class Entity {
    public:
        Entity()
        :   id(idGenerator++)
        {

        }

        uint32_t getId() const {
            return id;
        }

        void addComponent(Component* component) {
            components.push_back(component);
        }

        template <class T>
        T* getComponent() {
            for (int i = 0; i < components.size(); i++) {
                Component* c = components[i];

                if (dynamic_cast<T*>(c)) {
                    return dynamic_cast<T*>(c);
                }
            }
            throw ComponentNotFoundException();
        }

        template <class T>
        bool hasComponent() {
            for (int i = 0; i < components.size(); i++) {
                Component* c = components[i];

                if (dynamic_cast<T*>(c)) {
                    return true;
                }
            }
            return false;
        }

        const size_t numComponents() {
            return components.size();
        }

        std::string name;
    private:
        const uint32_t id;
        std::vector<Component*> components;
    };
}

#endif /* ENTITY_H */
