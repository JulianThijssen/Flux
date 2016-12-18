#include <Engine/Source/Entity.h>
#include <fstream>

namespace Flux {
    class Path;
    class Scene;

    class SceneConverter {
    public:
        static void convert(Path inputPath, Path outputPath);
    private:
        static void writeEntity(const Scene& scene, Entity* e, std::ofstream& out);
    };
}
