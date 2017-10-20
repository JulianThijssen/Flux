#pragma once
#ifndef MESH_H
#define MESH_H

#include "Component.h"
#include "Vector3f.h"
#include "Vector2f.h"

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

#endif /* MESH_H */
