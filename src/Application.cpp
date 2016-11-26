#include "Application.h"

#include <ctime>
#include <iostream>

namespace Flux {
    void Application::startGame() {
        currentScene.getMainCamera().getComponent<Transform>()->position.set(0, 0, 5);
        renderer.create();
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
