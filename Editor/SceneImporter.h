#pragma once
#ifndef SCENE_IMPORTER_H
#define SCENE_IMPORTER_H

namespace Flux {
    class Path;

    namespace Editor {
        class SceneDesc;

        class SceneImporter {
        public:
            static void loadScene(const Path path, SceneDesc& scene);
        };
    }
}

#endif /* SCENE_IMPORTER_H */
