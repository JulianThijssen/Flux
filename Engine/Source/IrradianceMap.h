#pragma once
#ifndef IRRADIANCE_MAP_H
#define IRRADIANCE_MAP_H

#include "Cubemap.h"
#include "Framebuffer.h"
#include "Shader.h"
#include "ShaderLoader.h"

#include <glad/glad.h>

namespace Flux {
    class IrradianceMap : public Cubemap {
    public:
        IrradianceMap(const Cubemap& environmentMap) : envMap(environmentMap) {}

        void generate(const unsigned int textureSize) {
            Shader* shader = ShaderLoader::loadShaderProgram("res/Quad.vert", "res/diffuseIBL.frag");

            Framebuffer framebuffer;
            framebuffer.bind();
            framebuffer.setDrawBuffer(GL_COLOR_ATTACHMENT0);

            glClearColor(1, 0, 0, 1);

            shader->bind();

            createEmpty(textureSize, false);

            glActiveTexture(GL_TEXTURE0);
            envMap.bind();
            shader->uniform1i("EnvMap", 0);

            shader->uniform1i("textureSize", envMap.getResolution());
            glViewport(0, 0, textureSize, textureSize);

            for (int i = 0; i < 6; i++) {
                shader->uniform1i("Face", i);
                framebuffer.setCubemap(*this, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0);
                framebuffer.validate();

                glClear(GL_COLOR_BUFFER_BIT);

                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            framebuffer.release();
        }
    private:
        const Cubemap& envMap;
    };
}

#endif /* IRRADIANCE_MAP_H */
