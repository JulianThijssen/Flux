#include "FpsCounter.h"

#include <ctime>

namespace Flux {
    void FpsCounter::init() {
        lastFpsCount = clock();
    }

    void FpsCounter::update() {
        frames++;

        double elapsed = double(clock() - lastFpsCount) / CLOCKS_PER_SEC;

        if (elapsed > 1) {
            lastFpsCount = clock();

            notifyListeners(frames);

            frames = 0;
        }
    }

    void FpsCounter::addListener(FpsListener& listener) {
        listeners.push_back(std::reference_wrapper<FpsListener>(listener));
    }

    void FpsCounter::notifyListeners(int framesPerSecond) {
        for (FpsListener& listener : listeners) {
            listener.onFpsUpdated(framesPerSecond);
        }
    }
}
