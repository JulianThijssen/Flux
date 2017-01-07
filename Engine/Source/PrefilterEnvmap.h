#pragma once
#ifndef PREFILTERED_ENVMAP_H
#define PREFILTERED_ENVMAP_H

#include "Cubemap.h"
#include "Framebuffer.h"
#include "Shader.h"
#include "ShaderLoader.h"

#include "Exceptions/ShaderCompilationException.h"
#include "Exceptions/ShaderLinkException.h"

#include <glad/glad.h>
#include <cmath>

namespace Flux {
    class PrefilterEnvmap : public Cubemap {
    public:
        PrefilterEnvmap(const Cubemap& environmentMap) : envMap(environmentMap) {}

        void generate() {
            Shader* shader;
            try {
                shader = ShaderLoader::loadShaderProgram("res/Quad.vert", "res/PrefilterEnvmap.frag");
            }
            catch (const ShaderCompilationException& e) {
                Log::error(e.what());
            }
            catch (const ShaderLinkException& e) {
                Log::error(e.what());
            }

            Framebuffer framebuffer;
            framebuffer.bind();
            framebuffer.setDrawBuffer(GL_COLOR_ATTACHMENT0);

            glClearColor(1, 0, 0, 1);

            shader->bind();

            createEmpty(envMap.getResolution(), true);

            glActiveTexture(GL_TEXTURE0);
            envMap.bind();
            shader->uniform1i("EnvMap", 0);

            const unsigned int MIP_MAP_LEVELS = 6;
            for (int level = 0; level < MIP_MAP_LEVELS; level++) {
                unsigned int mipmapSize = envMap.getResolution() >> level;
                glViewport(0, 0, mipmapSize, mipmapSize);
                float Roughness = (float) level / (MIP_MAP_LEVELS - 1);
                shader->uniform1f("Roughness", Roughness);

                for (int i = 0; i < 6; i++) {
                    shader->uniform1i("Face", i);
                    framebuffer.setCubemap(*this, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, level);
                    framebuffer.validate();

                    glClear(GL_COLOR_BUFFER_BIT);

                    glDrawArrays(GL_TRIANGLES, 0, 6);
                }
            }
            
            framebuffer.release();
        }
    private:
        const Cubemap& envMap;
    };
}

#endif /* PREFILTERED_ENVMAP_H */
