#pragma once

namespace Flux {
    class Size {
    public:
        Size(unsigned int width, unsigned int height)
            : width(width), height(height) { }

        void setSize(unsigned int width, unsigned int height) {
            this->width = width;
            this->height = height;
        }

        unsigned int width, height;
    };
}
