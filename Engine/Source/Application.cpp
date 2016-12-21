#include "Application.h"

#include <Editor/SceneConverter.h>
#include <Editor/SceneImporter.h>
#include <Editor/SceneDesc.h>

#include <Engine/Source/MeshRenderer.h> // Temp

#include "SceneLoader.h"
#include "Path.h"

#include <ctime>
#include <iostream>

namespace Flux {
    void Application::startGame() {
        SceneDesc scene;
        SceneImporter::loadScene(Path("res/Indoors.json"), scene);

        MeshRenderer* mr = new MeshRenderer();
        mr->materialID = 12;
        scene.entities[4]->addComponent(mr);
        
        SceneConverter::convert(scene, Path("res/Indoors.scene"));
        SceneLoader::loadScene(Path("res/Indoors.scene"), currentScene);
        bool created = renderer.create();
        if (!created)
            return;

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

            renderer.update(currentScene);

            window.update();
            frames++;
        }
    }
}

int main() {
    Flux::Application app;
    app.startGame();
}
