#include "Skybox.h"

#include "Util/Vector3f.h"

#include "Transform.h"
#include "Util/Matrix4f.h"

#include <glad/glad.h>

namespace Flux {
    Skybox::Skybox(char* const paths[6]) {
        create(paths, false);
    }

    void Skybox::render() {


    }
}
