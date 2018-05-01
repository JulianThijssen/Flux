#pragma once

namespace Flux {
    class Path;

    namespace Editor {
        class SceneDesc;

        class SceneImporter {
        public:
            enum class Status {
                Success, FileNotFound, Failure
            };

            static Status loadScene(const Path path, SceneDesc& scene);
        };
    }
}
