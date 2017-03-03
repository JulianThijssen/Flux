#pragma once
#ifndef SCRIPT_H
#define SCRIPT_H

#include "Scene.h"

namespace Flux {
    class Script {
    public:
        virtual void start(Scene& scene) = 0;
        virtual void update(Scene& scene) = 0;
    };
}

#endif /* SCRIPT_H */
