#include "Window.h"
#include "Scene.h"
#include "ForwardRenderer.h"

namespace Flux {
    class Application {
    public:
        Application() : window("Flux", 1024, 768), currentScene() { }

        void startGame();
        void update();

    private:
        Window window;
        Scene currentScene;
        ForwardRenderer renderer;

        int framesPerSecond;
        int maxSkip = 15;
        int skipTime = 40;
    };
}
