#include "Window.h"
#include "Scene.h"
#include "Renderer.h"

namespace Flux {
    class Application {
    public:
        void startGame();
        void update();

    private:
        Window window;
        Scene currentScene;
        Renderer renderer;

        int framesPerSecond;
        int maxSkip = 15;
        int skipTime = 40;
    };
}