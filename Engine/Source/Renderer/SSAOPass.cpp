#include "Renderer/SSAOPass.h"

#include "Renderer.h"
#include "Texture.h"
#include "TextureUnit.h"

#include "Entity.h"
#include "Transform.h"
#include "Camera.h"

#include "Util/Matrix4f.h"
#include "Util/Size.h"

#include <glad/glad.h>


#include "nvToolsExt.h"

namespace Flux {
    const unsigned int NUM_SAMPLES = 13;
    const unsigned int NOISE_SIZE = 4;

    namespace {
        Texture2D createRenderTexture(const Size& windowSize)
        {
            Texture2D renderTexture;
            renderTexture.create();
            renderTexture.bind(TextureUnit::TEXTURE0);
            renderTexture.setData(windowSize.width / 2, windowSize.height / 2, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            renderTexture.setWrapping(CLAMP, CLAMP);
            renderTexture.release();
            return renderTexture;
        }

        float random(float low, float high)
        {
            return low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
        }
    }

    SSAOPass::SSAOPass() : RenderPhase("SSAO"), windowSize(1, 1)
    {
        ssaoShader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/SSAO.frag");
        blurShader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/SSAOBlur.frag");

        generate();
    }

    void SSAOPass::generate()
    {
        // Generate a hemispherical kernel
        srand(0);
        kernel.resize(NUM_SAMPLES);

        kernel[0].set(0.0f, -0.92388f, 0.38268f);
        kernel[1].set(0.92388f, 0.0f, 0.38268f);
        kernel[2].set(0.0f, 0.92388f, 0.38268f);
        kernel[3].set(-0.92388f, 0.0f, 0.2f);
        kernel[4].set(0.5f, -0.5f, 0.70711f);
        kernel[5].set(0.5f, 0.5f, 0.70711f);
        kernel[6].set(-0.5f, 0.5f, 0.70711f);
        kernel[7].set(-0.5f, -0.5f, 0.70711f);
        kernel[8].set(0.0f, -0.38268f, 0.92388f);
        kernel[9].set(0.38268f, 0.0f, 0.92388f);
        kernel[10].set(0.0f, 0.38268f, 0.92388f);
        kernel[11].set(-0.38268f, 0.0f, 0.92388f);
        kernel[12].set(0.0f, 0.0f, 1.0f);

        for (int i = 0; i < NUM_SAMPLES; i++) {
            float scale = (float)i / NUM_SAMPLES;
            scale = (1 - scale*scale) * 0.1f + scale*scale;

            kernel[i] *= scale;
        }

        // Generate a noise texture
        noise.reserve(NOISE_SIZE*NOISE_SIZE);
        for (int i = 0; i < NOISE_SIZE*NOISE_SIZE; i++) {
            Vector3f v(random(-1, 1), random(-1, 1), 0.0f);
            v.normalise();
            v = v * 0.5 + Vector3f(0.5, 0.5, 0.5);
            noise.push_back(v);
        }

        noiseTexture.create();
        noiseTexture.bind(TextureUnit::TEXTURE0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        noiseTexture.setData(NOISE_SIZE, NOISE_SIZE, GL_RGB8, GL_RGB, GL_FLOAT, noise.data());
    }

    void SSAOPass::SetGBuffer(const GBuffer* gBuffer)
    {
        this->gBuffer = gBuffer;
    }

    void SSAOPass::Resize(const Size& windowSize)
    {
        this->windowSize = windowSize;

        buffer.create();
        buffer.bind();
        buffer.addColorTexture(0, createRenderTexture(windowSize));
        buffer.validate();
        buffer.release();
    }

    void SSAOPass::render(RenderState& renderState, const Scene& scene)
    {
        nvtxRangePushA(getPassName().c_str());

        const Framebuffer* sourceFramebuffer = RenderState::currentFramebuffer;
        ssaoShader.bind();

        ///
        Entity* camera = scene.getMainCamera();
        Transform* ct = camera->getComponent<Transform>();
        Camera* cam = camera->getComponent<Camera>();

        // Set the projection matrix from the camera parameters
        Matrix4f projMatrix;
        camera->getComponent<Camera>()->loadProjectionMatrix(projMatrix);

        // Set the view matrix to the camera view
        Matrix4f viewMatrix;
        viewMatrix.setIdentity();
        viewMatrix.rotate(-ct->rotation);
        viewMatrix.translate(-ct->position);

        ssaoShader.uniform3f("camPos", ct->position);
        ssaoShader.uniformMatrix4f("projMatrix", projMatrix);
        ssaoShader.uniformMatrix4f("viewMatrix", viewMatrix);
        ssaoShader.uniform1f("zNear", cam->getZNear());
        ssaoShader.uniform1f("zFar", cam->getZFar());
        ///

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glStencilFunc(GL_EQUAL, 1, 0xFF);

        glViewport(0, 0, windowSize.width / 2, windowSize.height / 2);

        gBuffer->normalTex.bind(TextureUnit::NORMAL);
        ssaoShader.uniform1i("normalMap", TextureUnit::NORMAL);
        gBuffer->positionTex.bind(TextureUnit::POSITION);
        ssaoShader.uniform1i("positionMap", TextureUnit::POSITION);
        gBuffer->depthTex.bind(TextureUnit::DEPTH);
        ssaoShader.uniform1i("depthMap", TextureUnit::DEPTH);

        noiseTexture.bind(TextureUnit::NOISE);
        ssaoShader.uniform1i("noiseMap", TextureUnit::NOISE);
        ssaoShader.uniform3fv("kernel", (int)kernel.size(), kernel.data());
        ssaoShader.uniform1i("kernelSize", (int)kernel.size());

        ssaoShader.uniform2i("windowSize", windowSize.width / 2, windowSize.height / 2);

        buffer.bind();
        renderState.drawQuad();

        // Blur
        nvtxRangePushA("SSAO Blur");
        blurShader.bind();
        blurShader.uniform2i("windowSize", windowSize.width, windowSize.height);

        buffer.getTexture().bind(TextureUnit::TEXTURE);
        blurShader.uniform1i("tex", TextureUnit::TEXTURE);

        renderState.drawQuad();
        nvtxRangePop();

        // Multiply
        sourceFramebuffer->bind();
        glViewport(0, 0, windowSize.width, windowSize.height);

        std::vector<Texture2D> sources{ *source, buffer.getTexture() };
        multiplyPass.SetTextures(sources);
        multiplyPass.render(renderState, scene);

        nvtxRangePop();
    }
}
