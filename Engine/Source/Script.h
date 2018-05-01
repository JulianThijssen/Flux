#pragma once

namespace Flux {
    class Scene;

    class Script {
    public:
        virtual void start(Scene& scene) = 0;
        virtual void update(Scene& scene) = 0;
    };
}
