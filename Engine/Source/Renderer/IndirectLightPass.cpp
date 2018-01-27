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
        shader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/DeferredIndirect.frag");

        if (scene.skybox) {
            iblSceneInfo.PrecomputeEnvironmentData(*scene.skybox);
        }
        else if (scene.skySphere) {
            iblSceneInfo.PrecomputeEnvironmentData(*scene.skySphere);
        }
        else {
            sky = false;
        }
    }

    void IndirectLightPass::SetGBuffer(const GBuffer* gBuffer)
    {
        this->gBuffer = gBuffer;
    }

    void IndirectLightPass::Resize(const Size& windowSize)
    {

    }

    void IndirectLightPass::render(RenderState& renderState, const Scene& scene)
    {
        renderState.setClearColor(0, 0, 0, 1);

        glClear(GL_COLOR_BUFFER_BIT);
        if (!sky) {
            return;
        }
        nvtxRangePushA(getPassName().c_str());
        shader.bind();

        Transform* ct = scene.getMainCamera()->getComponent<Transform>();
        shader.uniform3f("camPos", ct->position);

        gBuffer->albedoTex->bind(TextureUnit::ALBEDO);
        shader.uniform1i("albedoMap", TextureUnit::ALBEDO);
        gBuffer->normalTex->bind(TextureUnit::NORMAL);
        shader.uniform1i("normalMap", TextureUnit::NORMAL);
        gBuffer->positionTex->bind(TextureUnit::POSITION);
        shader.uniform1i("positionMap", TextureUnit::POSITION);
        gBuffer->depthTex->bind(TextureUnit::DEPTH);
        shader.uniform1i("depthMap", TextureUnit::DEPTH);

        iblSceneInfo.irradianceMap->bind(TextureUnit::IRRADIANCE);
        shader.uniform1i("irradianceMap", TextureUnit::IRRADIANCE);

        iblSceneInfo.prefilterEnvmap->bind(TextureUnit::PREFILTER);
        shader.uniform1i("prefilterEnvmap", TextureUnit::PREFILTER);

        iblSceneInfo.scaleBiasTexture->bind(TextureUnit::SCALEBIAS);
        shader.uniform1i("scaleBiasMap", TextureUnit::SCALEBIAS);

        renderState.drawQuad();

        nvtxRangePop();
    }
}
