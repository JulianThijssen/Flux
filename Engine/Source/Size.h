#pragma once
#ifndef FLUX_SIZE_H
#define FLUX_SIZE_H

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

#endif FLUX_SIZE_H
