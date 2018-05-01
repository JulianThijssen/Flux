#pragma once

#include "Script.h"
#include "Scene.h"

#include "Util/Vector2f.h"

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
