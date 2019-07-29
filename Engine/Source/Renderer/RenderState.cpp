#include "Renderer/RenderState.h"

#include "Framebuffer.h"
#include "Entity.h"
#include "Transform.h"
#include "Camera.h"
#include "Texture.h"

namespace Flux {
    GLuint RenderState::quadVao = 0;

    std::vector<uint> RenderState::textureUnits(Texture::MAX_TEXTURE_UNITS);
    uint RenderState::activeTextureUnit = 0;

    const Framebuffer* RenderState::currentFramebuffer = 0;

    RenderState::RenderState() :
        clearColor(1, 0, 1),
        projMatrix(),
        viewMatrix(),
        modelMatrix()
    {
        glGenVertexArrays(1, &quadVao);

        capabilityMap[BLENDING] = false;
        capabilityMap[FACE_CULLING] = false;
        capabilityMap[DEPTH_TEST] = false;
        capabilityMap[STENCIL_TEST] = false;
        capabilityMap[POLYGON_OFFSET] = false;
    }

    void RenderState::enable(Capability capability) {
        if (capabilityMap[capability])
            return;

        glEnable(capability);
        capabilityMap[capability] = true;
    }

    void RenderState::disable(Capability capability) {
        if (!capabilityMap[capability])
            return;

        glDisable(capability);
        capabilityMap[capability] = false;
    }

    void RenderState::require(CapabilitySet capabilitySet) {
        for (auto& pair : capabilitySet.getSet()) {
            if (pair.second)
                enable(pair.first);
            else
                disable(pair.first);
        }
    }

    void RenderState::setClearColor(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }

    void RenderState::drawQuad() const {
        glBindVertexArray(quadVao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void RenderState::setCamera(ShaderProgram& shader, Entity& camera) {
        Transform& ct = camera.getComponent<Transform>();
        Camera& cam = camera.getComponent<Camera>();

        setCamera(shader, ct, cam);
    }

    void RenderState::setCamera(ShaderProgram& shader, Transform& t, Camera& cam) {
        // Set the projection matrix from the camera parameters
        cam.loadProjectionMatrix(projMatrix);

        // Set the view matrix to the camera view
        viewMatrix.setIdentity();
        viewMatrix.rotate(-t.rotation);
        viewMatrix.translate(-t.position);

        shader.uniform3f("camPos", t.position);
        shader.uniformMatrix4f("projMatrix", projMatrix);
        shader.uniformMatrix4f("viewMatrix", viewMatrix);
        shader.uniform1f("zNear", cam.getZNear());
        shader.uniform1f("zFar", cam.getZFar());
    }

    GLuint RenderState::getActiveTexture()
    {
        return textureUnits[activeTextureUnit];
    }

    void RenderState::setActiveTexture(unsigned int textureUnit)
    {
        activeTextureUnit = textureUnit;

        glActiveTexture(GL_TEXTURE0 + textureUnit);
    }

    void RenderState::bindTexture(GLenum target, GLuint texture)
    {
        glBindTexture(target, texture);

        textureUnits[activeTextureUnit] = texture;
    }
}
