#include "Application.h"

#include "FluxConfig.h"

#include <Editor/SceneConverter.h>
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
    using namespace Editor;

    void Application::exportScene() {
        SceneDesc scene;
        SceneImporter::Status status = SceneImporter::loadScene(Path("res/Helmet.json"), scene);
        if (status == SceneImporter::Status::FileNotFound) {
            std::cout << "Failed to find scene file.";
            return;
        }

        SceneConverter::convert(scene, Path("res/Helmet.scene"));
        bool created = window.create();
        if (!created)
            return;
    }

    void Application::startGame() {
        std::cout << "Flux version " << Flux_VERSION_MAJOR << "." << Flux_VERSION_MINOR << std::endl;

        bool loaded = SceneLoader::loadScene(Path("res/Helmet.scene"), currentScene);
        if (!loaded)
            return;

#ifdef DEFERRED
        renderer = std::make_unique<DeferredRenderer>();
#else
        renderer = std::make_unique<ForwardRenderer>();
#endif
        bool created = renderer->create(currentScene, Size(window.getWidth(), window.getHeight()));
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
