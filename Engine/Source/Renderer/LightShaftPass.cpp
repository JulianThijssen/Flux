#include "LightShaftPass.h"

#include "Renderer/RenderState.h"

#include "Framebuffer.h"
#include "Texture.h"

#include "Transform.h"
#include "Camera.h"
#include "DirectionalLight.h"
#include "Util/Math.h"

namespace Flux
{
    LightShaftPass::LightShaftPass()
        :
        RenderPhase("LightShaft"),
        windowSize(1, 1),
        exposure(0.03f),
        density(1.6f),
        decay(0.9)
    {
        texShader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/Texture.frag");
        shader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/LightShaft.frag");

        requiredSet.addCapability(STENCIL_TEST, true);
        requiredSet.addCapability(DEPTH_TEST, false);
    }

    void LightShaftPass::Resize(const Size& windowSize)
    {
        this->windowSize = windowSize;

        lightTex.create();
        lightTex.bind(TextureUnit::TEXTURE0);
        lightTex.setData(windowSize.width / 2, windowSize.height / 2, GL_RGBA16F, GL_RGBA, GL_FLOAT, nullptr);
        lightTex.setWrapping(CLAMP, CLAMP);
        lightTex.setSampling(LINEAR, LINEAR);
        lightTex.release();

        buffer.create();
        buffer.bind();
        buffer.addColorTexture(0, lightTex);
        buffer.validate();
        buffer.release();
    }

    void LightShaftPass::setExposure(float exposure)
    {
        this->exposure = exposure;
    }

    void LightShaftPass::setDensity(float density)
    {
        this->density = density;
    }

    void LightShaftPass::setDecay(float decay) {
        this->decay = decay;
    }

    void LightShaftPass::render(RenderState& renderState, const Scene& scene)
    {
        renderState.require(requiredSet);

        nvtxRangePushA(getPassName().c_str());

        const Framebuffer* sourceFramebuffer = RenderState::currentFramebuffer;

        /** Render the non-occluded parts to the buffer, it will be used as input to the light shaft calculation */
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glStencilFunc(GL_EQUAL, 0, 0xFF);

        texShader.bind();

        source->bind(TextureUnit::TEXTURE0);
        texShader.uniform1i("tex", TextureUnit::TEXTURE0);

        renderState.drawQuad();

        renderState.disable(STENCIL_TEST);

        /** Light Shaft Pass */
        buffer.bind();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.bind();

        glViewport(0, 0, windowSize.width / 2, windowSize.height / 2);

        for (Entity* entity : scene.lights) {
            if (entity->hasComponent<DirectionalLight>()) {
                DirectionalLight& sun = entity->getComponent<DirectionalLight>();
                Transform& transform = entity->getComponent<Transform>();

                Vector3f direction = Math::directionFromRotation(transform.rotation, Vector3f(0, 0, -1));
                Vector3f position = renderState.projMatrix.transform(renderState.viewMatrix.transform(-direction * 10000, 1), 1);
                position = position * 0.5 + 0.5;

                shader.uniform2f("lightPos", position.x, position.y);
            }
        }

        sourceFramebuffer->getTexture().bind(TextureUnit::TEXTURE0);
        shader.uniform1i("sourceTex", TextureUnit::TEXTURE0);

        shader.uniform1f("exposure", exposure);
        shader.uniform1f("density", density);
        shader.uniform1f("decay", decay);

        renderState.drawQuad();

        /** Add the light shafts to the original input texture */
        sourceFramebuffer->bind();

        glViewport(0, 0, windowSize.width, windowSize.height);

        std::vector<Texture2D> sources{ *source, lightTex };
        std::vector<float> weights{ 1, 1 };
        addPass.SetTextures(sources);
        addPass.SetWeights(weights);
        addPass.render(renderState, scene);

        nvtxRangePop();
    }
}
