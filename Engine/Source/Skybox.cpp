#include "Skybox.h"

#include "Vector3f.h"

#include "Transform.h"
#include "Matrix4f.h"

#include <glad/glad.h>

namespace Flux {
    Skybox::Skybox(char* const paths[6]) {
        create(paths, false);
    }

    void Skybox::render() {


    }
}
