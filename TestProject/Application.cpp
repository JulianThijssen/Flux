#include "Application.h"

#include "FluxConfig.h"

#include <Editor/SceneExporter.h>
#include <Editor/SceneImporter.h>
#include <Editor/SceneDesc.h>

#include "ForwardRenderer.h"
#include "DeferredRenderer.h"
#include "FirstPersonView.h"
#include "SceneLoader.h"
#include "Util/Path.h"
#include "Util/Size.h"

#include <ctime>
#include <iostream>

#define DEFERRED

namespace Flux {
    void Application::exportScene() {
        Editor::SceneDesc scene;
        Editor::SceneImporter::Status importStatus = Editor::SceneImporter::loadScene(Path("res/Helmet.json"), scene);
        if (importStatus == Editor::SceneImporter::Status::FileNotFound) {
            std::cout << "Failed to find scene file.";
            return;
        }

        Editor::SceneExporter::Status exportStatus = Editor::SceneExporter::exportScene(scene, Path("res/Helmet.scene"));
        if (exportStatus == Editor::SceneExporter::Status::Failure) {
            std::cout << "Failed to export scene file.";
            return;
        }
    }

    void Application::startGame() {
        std::cout << "Flux version " << Flux_VERSION_MAJOR << "." << Flux_VERSION_MINOR << std::endl;

        bool created = window.create();
        if (!created)
            return;

        bool loaded = SceneLoader::loadScene(Path("res/Helmet.scene"), currentScene);
        if (!loaded)
            return;

#ifdef DEFERRED
        renderer = std::make_unique<DeferredRenderer>();
#else
        renderer = std::make_unique<ForwardRenderer>();
#endif
        created = renderer->create(currentScene, Size(window.getWidth(), window.getHeight()));
        if (!created)
            return;

        renderer->onResize(Size(window.getWidth(), window.getHeight()));

        currentScene.addScript(new FirstPersonView());

        fpsCounter.addListener(*this);

        update();
    }

    void Application::update() {
        clock_t nextUpdate = clock();
        fpsCounter.init();

        while (!window.isClosed()) {
            int skipped = 0;

            fpsCounter.update();

            while (clock() > nextUpdate && skipped < maxSkip) {
                currentScene.update();
                nextUpdate += skipTime;
                skipped++;
            }

            renderer->update(currentScene);
            window.update();
        }
    }

    void Application::onFpsUpdated(int framesPerSecond) {
        float ms = 1000.0f / framesPerSecond;

        std::stringstream ss;
        ss << "Flux      Fps: " << framesPerSecond << "   ms: " << ms;
        window.setTitle(ss.str().c_str());
    }
}

int main() {
    Flux::Application app;
    app.exportScene();
    app.startGame();
}
