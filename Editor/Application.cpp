#include "SceneImporter.h"
#include "SceneExporter.h"

#include "SceneDesc.h"
#include "Path.h"

#include <iostream>

namespace Flux
{
    namespace Editor
    {
        void exportScene() {
            SceneDesc scene;
            SceneImporter::Status importStatus = SceneImporter::loadScene(Path("res/TestScene.json"), scene);
            if (importStatus == Editor::SceneImporter::Status::FileNotFound) {
                std::cout << "Failed to find scene file.";
                return;
            }

            SceneExporter::Status exportStatus = SceneExporter::exportScene(scene, Path("res/TestScene.scene"));
            if (exportStatus == Editor::SceneExporter::Status::Failure) {
                std::cout << "Failed to export scene file.";
                return;
            }
        }
    }
}


int main() {
    Flux::Editor::exportScene();

    return 0;
}
