#pragma once
#ifndef FIRST_PERSON_VIEW_H
#define FIRST_PERSON_VIEW_H

#include "Script.h"
#include "Scene.h"

#include <Engine/Source/Vector2f.h>

namespace Flux {
    class FirstPersonView : public Script {
    public:
        virtual void start(Scene& scene);
        virtual void update(Scene& scene);
    private:
        Vector2f mousePos;

        float storedDX, storedDY;
    };
}

#endif /* FIRST_PERSON_VIEW_H */
