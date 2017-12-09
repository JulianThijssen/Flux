#include "Renderer/IndirectLightPass.h"

#include "Renderer/RenderState.h"

#include "TextureUnit.h"
#include "Texture.h"
#include "Framebuffer.h"

#include "Transform.h"
#include "Camera.h"

#include "DirectionalLight.h"

namespace Flux {
    IndirectLightPass::IndirectLightPass(const Scene& scene) : RenderPhase("Indirect Lighting")
    {
        shader = std::unique_ptr<Shader>(Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/DeferredIndirect.frag"));

        if (scene.skybox) {
            iblSceneInfo.PrecomputeEnvironmentData(*scene.skybox);
        }
        else if (scene.skySphere) {
            iblSceneInfo.PrecomputeEnvironmentData(*scene.skySphere);
        }
    }

    void IndirectLightPass::SetGBuffer(const GBuffer* gBuffer)
    {
        this->gBuffer = gBuffer;
    }

    void IndirectLightPass::SetTarget(const Framebuffer* target)
    {
        this->target = target;
    }

    void IndirectLightPass::render(const Scene& scene)
    {
        glClear(GL_COLOR_BUFFER_BIT);
        nvtxRangePushA(getPassName().c_str());
        shader->bind();

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

        shader->uniform3f("camPos", ct->position);
        shader->uniformMatrix4f("projMatrix", projMatrix);
        shader->uniformMatrix4f("viewMatrix", viewMatrix);
        shader->uniform1f("zNear", cam->getZNear());
        shader->uniform1f("zFar", cam->getZFar());
        ///

        gBuffer->albedoTex->bind(TextureUnit::ALBEDO);
        shader->uniform1i("albedoMap", TextureUnit::ALBEDO);
        gBuffer->normalTex->bind(TextureUnit::NORMAL);
        shader->uniform1i("normalMap", TextureUnit::NORMAL);
        gBuffer->positionTex->bind(TextureUnit::POSITION);
        shader->uniform1i("positionMap", TextureUnit::POSITION);
        gBuffer->depthTex->bind(TextureUnit::DEPTH);
        shader->uniform1i("depthMap", TextureUnit::DEPTH);

        iblSceneInfo.irradianceMap->bind(TextureUnit::IRRADIANCE);
        shader->uniform1i("irradianceMap", TextureUnit::IRRADIANCE);

        iblSceneInfo.prefilterEnvmap->bind(TextureUnit::PREFILTER);
        shader->uniform1i("prefilterEnvmap", TextureUnit::PREFILTER);

        iblSceneInfo.scaleBiasTexture->bind(TextureUnit::SCALEBIAS);
        shader->uniform1i("scaleBiasMap", TextureUnit::SCALEBIAS);

        RenderState::drawQuad();

        nvtxRangePop();
    }
}
