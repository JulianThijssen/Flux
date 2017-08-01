#ifndef MODEL_IMPORTER_H
#define MODEL_IMPORTER_H

#include <assimp/scene.h>

namespace Flux {
    class Path;

    namespace Editor {
        class Model;

        class ModelImporter {
        public:
            static Model* loadModel(const Path& path);
        private:
            static Model* copyModel(const aiScene& scene);
        };
    }
}

#endif /* MODEL_IMPORTER_H */
