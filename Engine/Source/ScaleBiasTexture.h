#pragma once
#ifndef SCALE_BIAS_TEXTURE_H
#define SCALE_BIAS_TEXTURE_H

#include <glad/glad.h>

#include "Framebuffer.h"
#include "TextureLoader.h"
#include "Texture.h"
#include "Shader.h"
#include "ShaderLoader.h"

#include "Exceptions/ShaderCompilationException.h"
#include "Exceptions/ShaderLinkException.h"
#include "Log.h"

namespace Flux {
    class ScaleBiasTexture : public Texture {
    public:
        ScaleBiasTexture()
        :   Texture(0)
        {
            glGenTextures(1, &handle);

            glBindTexture(GL_TEXTURE_2D, handle);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

            glBindTexture(GL_TEXTURE_2D, 0);
        }

        void generate()
        {
            Shader* shader;
            try {
                shader = ShaderLoader::loadShaderProgram("res/Quad.vert", "res/BRDFintegration.frag");
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

            glViewport(0, 0, 256, 256);

            framebuffer.setTexture(GL_COLOR_ATTACHMENT0, handle);
            framebuffer.validate();

            glClear(GL_COLOR_BUFFER_BIT);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            framebuffer.release();
        }
    };
}

#endif /* SCALE_BIAS_TEXTURE_H */
