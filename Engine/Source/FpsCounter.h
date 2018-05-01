#pragma once

#include "FpsListener.h"

#include <vector>
#include <ctime>

namespace Flux
{
    class FpsCounter
    {
    public:
        void init();
        void update();

        void addListener(FpsListener& listener);
    private:
        void notifyListeners(int framesPerSecond);

        std::vector<std::reference_wrapper<FpsListener>> listeners;

        clock_t lastFpsCount;
        int frames;
    };
}
