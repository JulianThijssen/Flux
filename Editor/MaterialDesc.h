#pragma once
#ifndef MATERIAL_DESC_H
#define MATERIAL_DESC_H

#include <string>

namespace Flux {
    class MaterialDesc {
    public:
        MaterialDesc(std::string path)
        : path(path)
        {

        }

        std::string path;
    };
}

#endif /* MATERIAL_DESC_H */
