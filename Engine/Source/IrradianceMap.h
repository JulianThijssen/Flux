#pragma once
#ifndef IRRADIANCE_MAP_H
#define IRRADIANCE_MAP_H

#include "Cubemap.h"
#include "Framebuffer.h"
#include "Shader.h"
#include "ShaderLoader.h"

namespace Flux {
    class IrradianceMap : public Cubemap {
    public:
        IrradianceMap(const Cubemap& environmentMap) : envMap(environmentMap) {}

        void generate(const unsigned int width, const unsigned int height) {
            Shader* shader = ShaderLoader::loadShaderProgram("res/diffuseIBL.vert", "res/diffuseIBL.frag");

            Framebuffer framebuffer;
            framebuffer.bind();
            framebuffer.setDrawBuffer(GL_COLOR_ATTACHMENT0);

            glClearColor(1, 0, 0, 1);

            shader->bind();

            createEmpty(width, height, false);

            glActiveTexture(GL_TEXTURE0);
            envMap.bind();
            shader->uniform1i("envmap", 0);

            glViewport(0, 0, width, height);

            for (int i = 0; i < 6; i++) {
                shader->uniform1i("face", i);
                framebuffer.setCubemap(*this, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
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
