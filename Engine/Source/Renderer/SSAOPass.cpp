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
    SSAOPass::SSAOPass() : RenderPhase("SSAO"), windowSize(1, 1)
    {
        ssaoShader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/SSAO.frag");
        blurShader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/SSAOBlur.frag");
    }

    void SSAOPass::SetGBuffer(const GBuffer* gBuffer)
    {
        this->gBuffer = gBuffer;
    }

    void SSAOPass::SetSsaoInfo(SsaoInfo* ssaoInfo)
    {
        this->ssaoInfo = ssaoInfo;
    }

    void SSAOPass::Resize(const Size& windowSize)
    {
        this->windowSize = windowSize;
    }

    void SSAOPass::render(RenderState& renderState, const Scene& scene)
    {
        nvtxRangePushA(getPassName().c_str());

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

        glViewport(0, 0, windowSize.width / 2, windowSize.height / 2);

        gBuffer->albedoTex->bind(TextureUnit::ALBEDO);
        ssaoShader.uniform1i("albedoMap", TextureUnit::ALBEDO);
        gBuffer->normalTex->bind(TextureUnit::NORMAL);
        ssaoShader.uniform1i("normalMap", TextureUnit::NORMAL);
        gBuffer->positionTex->bind(TextureUnit::POSITION);
        ssaoShader.uniform1i("positionMap", TextureUnit::POSITION);
        gBuffer->depthTex->bind(TextureUnit::DEPTH);
        ssaoShader.uniform1i("depthMap", TextureUnit::DEPTH);

        ssaoInfo->noiseTexture->bind(TextureUnit::NOISE);
        ssaoShader.uniform1i("noiseMap", TextureUnit::NOISE);
        ssaoShader.uniform3fv("kernel", (int)ssaoInfo->kernel.size(), ssaoInfo->kernel.data());
        ssaoShader.uniform1i("kernelSize", (int)ssaoInfo->kernel.size());

        ssaoShader.uniform2i("windowSize", windowSize.width / 2, windowSize.height / 2);

        ssaoInfo->getCurrentBuffer()->bind();
        renderState.drawQuad();

        nvtxRangePushA("SSAO Blur");
        blurShader.bind();
        blurShader.uniform2i("windowSize", windowSize.width, windowSize.height);

        ssaoInfo->getCurrentBuffer()->getColorTexture(0).bind(TextureUnit::TEXTURE);
        blurShader.uniform1i("tex", TextureUnit::TEXTURE);

        ssaoInfo->switchBuffers();
        ssaoInfo->getCurrentBuffer()->bind();
        renderState.drawQuad();

        glViewport(0, 0, windowSize.width, windowSize.height);
        nvtxRangePop();

        nvtxRangePop();
    }
}
