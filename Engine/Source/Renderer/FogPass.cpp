#include "Renderer/FogPass.h"

#include "Renderer/RenderState.h"

#include "TextureUnit.h"
#include "Texture.h"
#include "Framebuffer.h"

namespace Flux {
    FogPass::FogPass() : RenderPhase("Fog")
    {
        shader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/Fog.frag");
    }

    void FogPass::SetDepthMap(const Texture2D* depthMap)
    {
        this->depthMap = depthMap;
    }

    void FogPass::SetFogColor(const Vector3f fogColor)
    {
        this->fogColor = fogColor;
    }

    void FogPass::Resize(const Size& windowSize)
    {

    }

    void FogPass::render(RenderState& renderState, const Scene& scene)
    {
        nvtxRangePushA(getPassName().c_str());

        shader.bind();

        source->bind(TextureUnit::TEXTURE0);
        shader.uniform1i("tex", TextureUnit::TEXTURE0);
        depthMap->bind(TextureUnit::TEXTURE1);
        shader.uniform1i("depthMap", TextureUnit::TEXTURE1);

        Camera* camera = scene.getMainCamera()->getComponent<Camera>();
        shader.uniform1f("zNear", camera->getZNear());
        shader.uniform1f("zFar", camera->getZFar());
        shader.uniform3f("fogColor", fogColor);

        renderState.drawQuad();

        nvtxRangePop();
    }
}
