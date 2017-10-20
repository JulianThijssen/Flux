#pragma once
#ifndef SCENE_CONVERTER_H
#define SCENE_CONVERTER_H

#include "MaterialDesc.h"

namespace Flux {
    class Path;
    class Skysphere;

    namespace Editor {
        class SceneDesc;
        class Skybox;
        class Entity;

        struct Buffer {
            char* buf;
            int pos = 0;
        } typedef Buffer;

        class SceneConverter {
        public:
            static void convert(const SceneDesc& scene, Path outputPath);
        private:
            static void writeSkybox(Editor::Skybox* skybox, Buffer& buffer);
            static void writeSkysphere(Skysphere* skysphere, Buffer& buffer);
            static void writeMaterial(const uint32_t id, MaterialDesc* material, Buffer& buffer);
            static void writeEntity(const SceneDesc& scene, Entity* e, Buffer& buffer);
        };
    }
}

#endif /* SCENE_CONVERTER_H */
