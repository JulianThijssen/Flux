#ifndef MODELLOADER_HPP
#define MODELLOADER_HPP

#include <assimp/scene.h>

namespace Flux {
    class Model;
    class Path;

    class ModelLoader {
    public:
        static Model loadModel(const Path& path);
    private:
        static Model uploadModel(const aiScene& scene);
    };
}

#endif /* MODELLOADER_HPP */
