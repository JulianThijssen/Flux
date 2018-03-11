#include "Skybox.h"

#include "Util/Vector3f.h"

#include "Transform.h"
#include "Util/Path.h"
#include "Util/Matrix4f.h"

#include <glad/glad.h>

namespace Flux {
    Skybox::Skybox(std::vector<Path> paths) {
        loadFromFiles(paths);
    }

    void Skybox::render() {


    }
}
