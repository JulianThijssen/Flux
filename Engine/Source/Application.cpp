#include "Application.h"

#include <Editor/SceneConverter.h>
#include <Editor/SceneImporter.h>
#include <Editor/SceneDesc.h>

#include "ForwardRenderer.h"
#include "DeferredRenderer.h"
#include "FirstPersonView.h"
#include "SceneLoader.h"
#include "Path.h"

#include <ctime>
#include <iostream>

#define DEFERRED

namespace Flux {
    void Application::startGame() {
        SceneDesc scene;
        SceneImporter::loadScene(Path("res/Indoors.json"), scene);

        SceneConverter::convert(scene, Path("res/Indoors.scene"));
        SceneLoader::loadScene(Path("res/Indoors.scene"), currentScene);

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

        update();
    }

    void Application::update() {
        int frames = 0;

        clock_t nextUpdate = clock();
        clock_t lastFpsCount = clock();

        while (!window.isClosed()) {
            int skipped = 0;

            double elapsed = double(clock() - lastFpsCount) / CLOCKS_PER_SEC;

            if (elapsed > 1) {
                lastFpsCount = clock();
                framesPerSecond = frames;
                std::cout << frames << std::endl;
                frames = 0;
            }

            while (clock() > nextUpdate && skipped < maxSkip) {
                currentScene.update();
                nextUpdate += skipTime;
                skipped++;
            }

            renderer->update(currentScene);

            window.update();
            frames++;
        }
    }
}

int main() {
    Flux::Application app;
    app.startGame();
}
