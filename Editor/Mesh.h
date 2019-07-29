#pragma once

#include "Component.h"
#include "Util/Vector2f.h"

#include <GDT/Vector3f.h>

#include <vector>
#include <string>

using GDT::Vector3f;

namespace Flux {
    namespace Editor {
        class Mesh : public Component {
        public:
            std::string name;
            std::vector<Vector3f> vertices;
            std::vector<Vector2f> texCoords;
            std::vector<Vector3f> normals;
            std::vector<Vector3f> tangents;
            std::vector<unsigned int> indices;

            std::string materialName;
        };
    }
}

