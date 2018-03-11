#pragma once
#ifndef SKYBOX_H
#define SKYBOX_H

#include "Texture.h"

#include <vector>

namespace Flux {
    class Path;

    class Skybox : public Cubemap {
    public:
        Skybox(std::vector<Path> paths);

        void render();
    private:
        unsigned int cube;
    };
}

#endif /* SKYBOX_H */
