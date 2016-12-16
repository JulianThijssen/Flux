#pragma once
#ifndef SCENE_IMPORTER_H
#define SCENE_IMPORTER_H

namespace Flux {
    class Path;
    class Scene;

    class SceneImporter {
    public:
        static void loadScene(const Path path, Scene& scene);
    };
}

#endif /* SCENE_IMPORTER_H */
