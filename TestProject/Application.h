#include "Window.h"
#include "Scene.h"
#include "Renderer.h"

#include "FpsListener.h"
#include "FpsCounter.h"

#include <memory>

namespace Flux {
    class Application : protected FpsListener {
    public:
        Application() : window("Flux", 1920, 1080) { }

        void startGame();
        void update();
        void onFpsUpdated(int framesPerSecond) override;

    private:
        Window window;
        Scene currentScene;
        std::unique_ptr<Renderer> renderer;

        FpsCounter fpsCounter;

        int maxSkip = 15;
        int skipTime = 40;
    };
}
