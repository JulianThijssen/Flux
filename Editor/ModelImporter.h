#ifndef MODEL_IMPORTER_H
#define MODEL_IMPORTER_H

#include <assimp/scene.h>

namespace Flux {
    class Model;
    class Path;

    class ModelImporter {
    public:
        static Model* loadModel(const Path& path);
    private:
        static Model* uploadModel(const aiScene& scene);
    };
}

#endif /* MODEL_IMPORTER_H */
