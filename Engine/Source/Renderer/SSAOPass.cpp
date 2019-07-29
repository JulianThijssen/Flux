#include "Renderer/SSAOPass.h"

#include "Renderer.h"
#include "Texture.h"
#include "TextureUnit.h"

#include "Entity.h"
#include "Transform.h"
#include "Camera.h"

#include "Util/Vector2f.h"
#include "Util/Size.h"

#include <GDT/Matrix4f.h>

#include <glad/glad.h>

#include "nvToolsExt.h"

namespace Flux {
    const unsigned int NOISE_SIZE = 4;

    namespace {
        Texture2D createRenderTexture(const Size& windowSize)
        {
            Texture2D renderTexture;
            renderTexture.create();
            renderTexture.bind(TextureUnit::TEXTURE0);
            renderTexture.setData(windowSize.width / 2, windowSize.height / 2, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            renderTexture.setWrapping(CLAMP, CLAMP);
            renderTexture.setSampling(LINEAR, LINEAR);
            renderTexture.release();
            return renderTexture;
        }

        float random(float low, float high)
        {
            return low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
        }

        float lerp(float a, float b, float t)
        {
            return (1 - t) * a + t * b;
        }

        float RadicalInverse(uint bits) {
            bits = (bits << 16u) | (bits >> 16u);
            bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
            bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
            bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
            bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
            return float(bits) * 2.3283064365386963e-10; // / 0x100000000
        }

        Vector2f Hammersley(uint i, uint NumSamples) {
            return Vector2f(float(i) / float(NumSamples), RadicalInverse(i));
        }

        Vector3f ImportanceSampleGGX(Vector2f Xi, float Roughness, Vector3f N) {
            float a = Roughness * Roughness;

            float Phi = 2 * 3.14159265f * Xi.x;

            float CosTheta = sqrt((1 - Xi.y) / (1 + (a*a - 1) * Xi.y));
            float SinTheta = sqrt(1 - CosTheta * CosTheta);

            Vector3f H(SinTheta * cos(Phi), SinTheta * sin(Phi), CosTheta);

            return H;
        }

        std::vector<Vector3f> GenerateKernel(int NumSamples) {
            std::vector<Vector3f> kernel(NumSamples);

            for (int i = 0; i < NumSamples; i++) {
                kernel[i].set(random(-1, 1), random(-1, 1), random(0, 1));
                kernel[i].normalize();
            }

            for (int i = 0; i < NumSamples; i++) {
                float scale = (float)i / NumSamples;
                scale = lerp(0.1f, 1.0f, scale * scale);

                kernel[i] *= scale;
            }

            return kernel;
        }

        std::vector<Vector3f> GenerateKernelGGX(int NumSamples) {
            std::vector<Vector3f> kernel(NumSamples);
            kernel.resize(NumSamples);

            for (int i = 0; i < NumSamples; i++) {
                Vector2f Xi = Hammersley(i, NumSamples);
                Vector3f H = ImportanceSampleGGX(Xi, 1, Vector3f(0, 0, 1));
                float R = RadicalInverse(i) * 1.0 + 0.1f;
                kernel[i].set(H * R);
            }

            return kernel;
        }
    }

    SSAOPass::SSAOPass() : RenderPhase("SSAO"), windowSize(1, 1)
    {
        ssaoShader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/SSAO.frag");
        blurShader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/SSAOBlur.frag");

        generate();

        requiredSet.addCapability(STENCIL_TEST, true);
        requiredSet.addCapability(DEPTH_TEST, false);
    }

    void SSAOPass::generate()
    {
        kernel = GenerateKernelGGX(32);

        // Generate a noise texture
        noise.reserve(NOISE_SIZE*NOISE_SIZE);
        for (int i = 0; i < NOISE_SIZE*NOISE_SIZE; i++) {
            Vector3f v(random(-1, 1), random(-1, 1), 0.0f);
            v.normalize();
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
        buffer.addColorTexture(1, createRenderTexture(windowSize));
        buffer.validate();
        buffer.release();
    }

    void SSAOPass::render(RenderState& renderState, const Scene& scene)
    {
        renderState.require(requiredSet);

        nvtxRangePushA(getPassName().c_str());

        const Framebuffer* sourceFramebuffer = RenderState::currentFramebuffer;
        ssaoShader.bind();

        ///
        Entity* camera = scene.getMainCamera();
        Transform& ct = camera->getComponent<Transform>();
        Camera& cam = camera->getComponent<Camera>();

        // Set the projection matrix from the camera parameters
        Matrix4f projMatrix;
        cam.loadProjectionMatrix(projMatrix);

        // Set the view matrix to the camera view
        Matrix4f viewMatrix;
        viewMatrix.setIdentity();
        viewMatrix.rotate(-ct.rotation);
        viewMatrix.translate(-ct.position);

        ssaoShader.uniform3f("camPos", ct.position);
        ssaoShader.uniformMatrix4f("projMatrix", projMatrix);
        ssaoShader.uniformMatrix4f("viewMatrix", viewMatrix);
        ///

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
        buffer.setDrawBuffer(0);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glViewport(0, 0, windowSize.width / 2, windowSize.height / 2);
        renderState.drawQuad();

        // Blur
        nvtxRangePushA("SSAO Blur");
        blurShader.bind();
        blurShader.uniform2i("windowSize", windowSize.width, windowSize.height);

        buffer.getTexture().bind(TextureUnit::TEXTURE);
        blurShader.uniform1i("tex", TextureUnit::TEXTURE);

        buffer.setDrawBuffer(1);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
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
