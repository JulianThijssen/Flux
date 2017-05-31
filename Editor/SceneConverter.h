#include "MaterialDesc.h"

#include <Engine/Source/Entity.h>
#include <fstream>

namespace Flux {
    class Path;
    class SceneDesc;
    class Skysphere;

    namespace Editor {
        class Skybox;
    }

    class SceneConverter {
    public:
        static void convert(const SceneDesc& scene, Path outputPath);
    private:
        static void writeSkybox(Editor::Skybox* skybox, std::ofstream& out);
        static void writeSkysphere(Skysphere* skysphere, std::ofstream& out);
        static void writeMaterial(const uint32_t id, MaterialDesc* material, std::ofstream& out);
        static void writeEntity(const SceneDesc& scene, Entity* e, std::ofstream& out);
    };
}
