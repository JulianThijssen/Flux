#include "Skybox.h"

#include <Engine/Source/Vector3f.h>

#include <glad/glad.h>

namespace Flux {
    Skybox::Skybox(const char* paths[6]) {
        Vector3f vertices[8]
        {
            Vector3f(-1, -1, 1),
            Vector3f(1, -1, 1),
            Vector3f(-1, 1, 1),
            Vector3f(1, 1, 1),
            Vector3f(-1, -1, -1),
            Vector3f(1, -1, -1),
            Vector3f(-1, 1, -1),
            Vector3f(1, 1, -1)
        };

        unsigned int indices[36]
        {
            0, 1, 2, 2, 1, 3,
            1, 5, 3, 3, 5, 7,
            5, 4, 7, 7, 4, 6,
            4, 0, 6, 6, 0, 2,
            2, 3, 6, 6, 3, 7,
            4, 5, 0, 0, 5, 1
        };

        // Generate vertex array object
        glGenVertexArrays(1, &cube);
        glBindVertexArray(cube);

        // Store faces in a buffer
        GLuint faceVBO;
        glGenBuffers(1, &faceVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceVBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

        // Store vertices in a buffer
        GLuint vertexVBO;
        glGenBuffers(1, &vertexVBO);
        glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(Vector3f), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
        glEnableVertexAttribArray(0);

        create(paths, false);
    }

    void Skybox::render() {
        glDisable(GL_CULL_FACE);
        glDepthFunc(GL_LEQUAL);

        glBindVertexArray(cube);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

        glEnable(GL_CULL_FACE);
        glDepthFunc(GL_LESS);
    }
}
