#include "Skybox.h"

#include "Transform.h"
#include "Util/Path.h"

#include <GDT/Vector3f.h>
#include <GDT/Matrix4f.h>

#include <glad/glad.h>

namespace Flux {
    Skybox::Skybox(std::vector<Path> paths) {
        loadFromFiles(paths);
    }

    void Skybox::render() {


    }
}
