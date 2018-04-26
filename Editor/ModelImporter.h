#pragma once

#include <assimp/scene.h>

#include <memory>

namespace Flux {
    class Path;

    namespace Editor {
        class Model;

        class ModelImporter {
        public:
            Model ImportFromFile(const Path& path);
        private:
            Model ReadModelFromScene(const aiScene& scene);
        };
    }
}
