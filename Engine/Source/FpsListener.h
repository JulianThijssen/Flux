#pragma once
#ifndef FPS_LISTENER_H
#define FPS_LISTENER_H

namespace Flux
{
    class FpsListener
    {
    public:
        virtual void onFpsUpdated(int framesPerSecond) = 0;
    };
}

#endif /* FPS_LISTENER_H */
