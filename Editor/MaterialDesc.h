#pragma once

#include <string>

namespace Flux {
    class MaterialDesc {
    public:
        MaterialDesc(std::string name, std::string path)
        :   name(name)
        ,   path(path)
        {

        }

        std::string name;
        std::string path;
    };
}

