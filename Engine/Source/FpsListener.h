#pragma once

namespace Flux
{
    class FpsListener
    {
    public:
        virtual void onFpsUpdated(int framesPerSecond) = 0;
    };
}
