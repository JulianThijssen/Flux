#pragma once
#ifndef ENTITY_H
#define ENTITY_H

#include "Component.h"
#include "Exceptions/ComponentNotFoundException.h"

#include <vector>

namespace Flux {
    class Entity {
    public:
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

    private:
        std::vector<Component*> components;
    };
}

#endif /* ENTITY_H */
