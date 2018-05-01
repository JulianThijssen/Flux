#pragma once

#include "Component.h"
#include "Util/Vector3f.h"
#include "Util/Vector2f.h"

#include <vector>
#include <string>

namespace Flux {
    class Mesh : public Component {
    public:
        std::string name;
        std::vector<Vector3f> vertices;
        std::vector<Vector2f> texCoords;
        std::vector<Vector3f> normals;
        std::vector<Vector3f> tangents;
        std::vector<unsigned int> indices;

        unsigned int handle;
        unsigned int indexBuffer;
        std::string materialName;
    };
}
