#include "Renderer/LtcLightPass.h"

#include "Renderer/RenderState.h"

#include "TextureUnit.h"
#include "Texture.h"
#include "Framebuffer.h"
#include "TextureLoader.h"
#include "Entity.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "GGX.h"

#include "DirectionalLight.h"
#include "AttachedTo.h"
#include <iostream>

namespace Flux {
    LtcLightPass::LtcLightPass() : RenderPhase("Area Lighting")
    {
        shader = std::unique_ptr<Shader>(Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/DeferredArea.frag"));

        ampTex = TextureLoader::create(64, 64, GL_R32F, GL_RED, GL_FLOAT, CLAMP, SamplingConfig(LINEAR, LINEAR, NONE), amp.data());
        std::vector<float> data;
        data.reserve(a.size() + b.size() + c.size() + d.size());
        for (int i = 0; i < 64 * 64; i++) {
            data.push_back(a[i]);
            data.push_back(b[i]);
            data.push_back(c[i]);
            data.push_back(d[i]);
        }

        matTex = TextureLoader::create(64, 64, GL_RGBA32F, GL_RGBA, GL_FLOAT, CLAMP, SamplingConfig(LINEAR, LINEAR, NONE), data.data());
    }

    void LtcLightPass::SetGBuffer(const GBuffer* gBuffer)
    {
        this->gBuffer = gBuffer;
    }

    void LtcLightPass::SetTarget(const Framebuffer* target)
    {
        this->target = target;
    }

    void LtcLightPass::render(RenderState& renderState, const Scene& scene)
    {
        nvtxRangePushA(getPassName().c_str());

        shader->bind();

        renderState.enable(BLENDING);
        glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ZERO);
        glDepthFunc(GL_LEQUAL);

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

        ampTex->bind(TextureUnit::TEXTURE3);
        matTex->bind(TextureUnit::TEXTURE4);
        shader->uniform1i("ampTex", TextureUnit::TEXTURE3);
        shader->uniform1i("matTex", TextureUnit::TEXTURE4);

        for (Entity* e : scene.entities) {
            if (!e->hasComponent<Mesh>())
                continue;

            if (e->hasComponent<MeshRenderer>()) {
                MeshRenderer* mr = e->getComponent<MeshRenderer>();
                Transform* transform = e->getComponent<Transform>();
                Material* material = scene.materials[mr->materialID];

                if (material && material->emission.length() > 0.01) {
                    Matrix4f modelMatrix;
                    modelMatrix.setIdentity();

                    if (e->hasComponent<AttachedTo>()) {
                        Entity* parent = scene.getEntityById(e->getComponent<AttachedTo>()->parentId);

                        if (parent != nullptr) {
                            Transform* parentT = parent->getComponent<Transform>();
                            modelMatrix.translate(parentT->position);
                            modelMatrix.rotate(parentT->rotation);
                            modelMatrix.scale(parentT->scale);
                        }
                    }

                    modelMatrix.translate(transform->position);
                    modelMatrix.rotate(transform->rotation);
                    modelMatrix.scale(transform->scale);

                    Mesh* mesh = e->getComponent<Mesh>();

                    std::vector<Vector3f> vertices;
                    for (int i = 0; i < mesh->vertices.size(); i++) {
                        vertices.push_back(modelMatrix.transform(mesh->vertices[i], 1));
                    }
                    Vector3f t = vertices[3];
                    vertices[3] = vertices[2];
                    vertices[2] = t;

                    shader->uniform1i("numVertices", vertices.size());
                    shader->uniform3fv("vertices", vertices.size(), vertices.data());
                    shader->uniform3f("emission", material->emission);

                    renderState.drawQuad();
                }
            }
        }

        renderState.disable(BLENDING);

        nvtxRangePop();
    }
}
