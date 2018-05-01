#pragma once

namespace Flux {
    class Path;
    class Scene;

    class SceneLoader {
    public:
        static bool loadScene(const Path path, Scene& scene);
    };
}
