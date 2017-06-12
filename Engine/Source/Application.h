#include "Window.h"
#include "Scene.h"
#include "Renderer.h"

#include <memory>

namespace Flux {
    class Application {
    public:
        Application() : window("Flux", 1920, 1080) { }

        void startGame();
        void update();

    private:
        Window window;
        Scene currentScene;
        std::unique_ptr<Renderer> renderer;

        int framesPerSecond;
        int maxSkip = 15;
        int skipTime = 40;
    };
}
