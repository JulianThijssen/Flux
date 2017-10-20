#include "SkyPass.h"

#include <Engine/Source/Renderer.h>

#include <Engine/Source/TextureUnit.h>

#include <Engine/Source/Transform.h>
#include <Engine/Source/Camera.h>

namespace Flux {
    SkyPass::SkyPass() : RenderPhase("Sky")
    {
        skyboxShader = std::unique_ptr<Shader>(Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/Skybox.frag"));
        skysphereShader = std::unique_ptr<Shader>(Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/Sky.frag"));
    }

    void SkyPass::render(const Scene& scene)
    {
        nvtxRangePushA(getPassName().c_str());

        Transform* transform = scene.mainCamera->getComponent<Transform>();
        Camera* cam = scene.mainCamera->getComponent<Camera>();

        // Set the projection matrix from the camera parameters
        Matrix4f projMatrix;
        cam->loadProjectionMatrix(projMatrix);

        Matrix4f yawMatrix;
        yawMatrix.rotate(transform->rotation.y, 0, 1, 0);

        Matrix4f pitchMatrix;
        pitchMatrix.rotate(transform->rotation.x, 1, 0, 0);

        Matrix4f cameraBasis;
        cameraBasis[10] = -1;
        cameraBasis = yawMatrix * pitchMatrix * cameraBasis;

        Shader* shader;
        if (scene.skybox) {
            shader = skyboxShader.get();
            shader->bind();
            scene.skybox->bind(TextureUnit::TEXTURE);
            shader->uniform1i("skybox", TextureUnit::TEXTURE);
        }
        else if (scene.skySphere) {
            shader = skysphereShader.get();
            shader->bind();
            scene.skySphere->bind(TextureUnit::TEXTURE);
            shader->uniform1i("tex", TextureUnit::TEXTURE);
            shader->uniform3f("sun", -0.471409702f, -0.5061866455f, 0.722182783f);
        }
        else {
            return;
        }

        shader->uniform2f("persp", 1.0f / projMatrix.toArray()[0], 1.0f / projMatrix.toArray()[5]);
        shader->uniformMatrix4f("cameraBasis", cameraBasis);

        glDepthFunc(GL_LEQUAL);
        glBindVertexArray(Renderer::quadVao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDepthFunc(GL_LESS);

        nvtxRangePop();
    }
}
