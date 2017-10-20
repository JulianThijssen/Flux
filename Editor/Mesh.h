#pragma once
#ifndef EDITOR_MESH_H
#define EDITOR_MESH_H

#include "Component.h"
#include "Vector3f.h"
#include "Vector2f.h"

#include <vector>
#include <string>

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

#endif /* EDITOR_MESH_H */
