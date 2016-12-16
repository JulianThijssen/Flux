#include "SceneConverter.h"

#include "SceneImporter.h"

#include "../Engine/Source/Path.h"
#include "../Engine/Source/Scene.h"

#include "../Engine/Source/Transform.h"
#include "../Engine/Source/Mesh.h"
#include "../Engine/Source/AttachedTo.h"

#include <fstream>
#include <iostream> // Temp

namespace Flux {
    void SceneConverter::convert(Path inputPath, Path outputPath) {
        Scene scene;
        SceneImporter::loadScene(inputPath, scene);

        ofstream outFile;
        outFile.open(outputPath.str().c_str(), ios::out | ios::binary);

        const uint32_t numEntities = (uint32_t) scene.entities.size();

        outFile.write((char *) &numEntities, sizeof(numEntities));

        for (Entity* e : scene.entities) {
            const uint32_t numComponents = e->hasComponent<Mesh>() ? 2 : 1;
            outFile.write((char *) &numComponents, sizeof(numComponents));
            if (e->hasComponent<Transform>()) {
                outFile.write("t", sizeof(char));
                Transform* t = e->getComponent<Transform>();

                if (e->hasComponent<AttachedTo>()) {
                    AttachedTo* at = e->getComponent<AttachedTo>();
                    Transform* pt = at->parent->getComponent<Transform>();

                    Vector3f pos = t->position + pt->position;
                    Vector3f rot = t->rotation + pt->rotation;
                    Vector3f scale = t->scale * pt->scale;
                    outFile.write((char *)&pos, sizeof(Vector3f));
                    outFile.write((char *)&rot, sizeof(Vector3f));
                    outFile.write((char *)&scale, sizeof(Vector3f));
                }
                else {
                    outFile.write((char *)&t->position, sizeof(Vector3f));
                    outFile.write((char *)&t->rotation, sizeof(Vector3f));
                    outFile.write((char *)&t->scale, sizeof(Vector3f));
                }
            }
            if (e->hasComponent<Mesh>()) {
                outFile.write("m", sizeof(char));
                Mesh* mesh = e->getComponent<Mesh>();

                const uint32_t numVertices = (uint32_t) mesh->vertices.size();
                const uint32_t numTexCoords = (uint32_t)mesh->texCoords.size();
                const uint32_t numNormals = (uint32_t)mesh->normals.size();
                const uint32_t numIndices = (uint32_t)mesh->indices.size();

                outFile.write((char *) &numVertices, sizeof(numVertices));
                outFile.write((char *) &mesh->vertices[0], numVertices * sizeof(Vector3f));
                
                outFile.write((char *) &numTexCoords, sizeof(numTexCoords));
                outFile.write((char *) &mesh->texCoords[0], numTexCoords * sizeof(Vector2f));
                
                outFile.write((char *) &numNormals, sizeof(numNormals));
                outFile.write((char *) &mesh->normals[0], numNormals * sizeof(Vector3f));
                
                outFile.write((char *) &numIndices, sizeof(numIndices));
                outFile.write((char *) &mesh->indices[0], numIndices * sizeof(unsigned int));
            }
            if (e->hasComponent<Camera>()) {
                outFile.write("c", sizeof(char));
                Camera* camera = e->getComponent<Camera>();

                const float fieldOfView = camera->getFovy();
                const float aspectRatio = camera->getAspectRatio();
                const float zNear = camera->getZNear();
                const float zFar = camera->getZFar();

                outFile.write((char *) &fieldOfView, sizeof(float));
                outFile.write((char *) &aspectRatio, sizeof(float));
                outFile.write((char *) &zNear, sizeof(float));
                outFile.write((char *) &zFar, sizeof(float));
            }
        }
    }
}
