#pragma once
#ifndef SCENE_LOADER_H
#define SCENE_LOADER_H

namespace Flux {
    class Path;
    class Scene;

    class SceneLoader {
    public:
        static void loadScene(const Path path, Scene& scene);
    };
}

#endif /* SCENE_LOADER_H */
