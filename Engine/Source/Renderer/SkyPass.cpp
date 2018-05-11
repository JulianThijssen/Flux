#include "Renderer/SkyPass.h"

#include "Renderer.h"

#include "TextureUnit.h"

#include "Transform.h"
#include "Camera.h"
#include "DirectionalLight.h"
#include "Util/Math.h"

namespace Flux {
    SkyPass::SkyPass() : RenderPhase("Sky")
    {
        skyboxShader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/Skybox.frag");
        skysphereShader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/Skysphere.frag");
        skyShader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/Sky.frag");
        texShader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/Texture.frag");

        requiredSet.addCapability(STENCIL_TEST, true);
        requiredSet.addCapability(DEPTH_TEST, false);
    }

    void SkyPass::Resize(const Size& windowSize)
    {

    }

    void SkyPass::render(RenderState& renderState, const Scene& scene)
    {
        renderState.require(requiredSet);

        glStencilFunc(GL_EQUAL, 0, 0xFF);

        Transform& transform = scene.mainCamera->getComponent<Transform>();
        Camera& cam = scene.mainCamera->getComponent<Camera>();

        // Set the projection matrix from the camera parameters
        Matrix4f projMatrix;
        cam.loadProjectionMatrix(projMatrix);

        Matrix4f yawMatrix;
        yawMatrix.rotate(transform.rotation.y, 0, 1, 0);

        Matrix4f pitchMatrix;
        pitchMatrix.rotate(transform.rotation.x, 1, 0, 0);

        Matrix4f cameraBasis;
        cameraBasis[10] = -1;
        cameraBasis = yawMatrix * pitchMatrix * cameraBasis;

        Shader& shader = skyShader;
        if (scene.skybox) { shader = skyboxShader; }
        else if (scene.skySphere) { shader = skysphereShader; }

        if (scene.skybox) {
            shader.bind();
            scene.skybox->bind(TextureUnit::TEXTURE);
            shader.uniform1i("skybox", TextureUnit::TEXTURE);
        }
        else if (scene.skySphere) {
            shader.bind();
            scene.skySphere->bind(TextureUnit::TEXTURE);
            shader.uniform1i("tex", TextureUnit::TEXTURE);
        }
        else {
            shader.bind();

            for (Entity* entity : scene.lights) {
                if (entity->hasComponent<DirectionalLight>()) {
                    DirectionalLight& sun = entity->getComponent<DirectionalLight>();
                    Transform& sunTransform = scene.mainCamera->getComponent<Transform>();
                    
                    Vector3f direction = Math::directionFromRotation(sunTransform.rotation, Vector3f(0, 0, -1));
                    shader.uniform3f("sun", direction);
                }
            }
        }

        nvtxRangePushA(getPassName().c_str());

        glDepthMask(GL_TRUE);
        shader.uniform2f("persp", 1.0f / projMatrix.toArray()[0], 1.0f / projMatrix.toArray()[5]);
        shader.uniformMatrix4f("cameraBasis", cameraBasis);

        glDepthFunc(GL_LEQUAL);
        renderState.drawQuad();
        glDepthFunc(GL_LESS);

        glDepthMask(GL_FALSE);


        // Draw source
        glStencilFunc(GL_EQUAL, 1, 0xFF);

        texShader.bind();

        source->bind(TextureUnit::TEXTURE0);
        texShader.uniform1i("tex", TextureUnit::TEXTURE0);

        renderState.drawQuad();

        nvtxRangePop();
    }
}
