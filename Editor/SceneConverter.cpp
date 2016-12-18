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

        std::ofstream outFile;
        outFile.open(outputPath.str().c_str(), ios::out | ios::binary);

        const uint32_t numEntities = (uint32_t)(scene.entities.size() + scene.lights.size() + (scene.mainCamera ? 1 : 0));

        outFile.write((char *) &numEntities, sizeof(numEntities));

        for (Entity* e : scene.entities) {
            writeEntity(scene, e, outFile);
        }
        for (Entity* light : scene.lights) {
            writeEntity(scene, light, outFile);
        }
        writeEntity(scene, scene.mainCamera, outFile);

        outFile.close();
    }

    void SceneConverter::writeEntity(const Scene& scene, Entity* e, std::ofstream& out) {
        const uint32_t id = (uint32_t)e->getId();
        out.write((char *)&id, sizeof(id));
        const uint32_t numComponents = (uint32_t)e->numComponents();
        out.write((char *)&numComponents, sizeof(numComponents));
        if (e->hasComponent<Transform>()) {
            out.write("t", sizeof(char));
            Transform* t = e->getComponent<Transform>();

            if (e->hasComponent<AttachedTo>()) {
                AttachedTo* at = e->getComponent<AttachedTo>();
                Entity* parent = scene.getEntityById(at->parentId);
                Transform* pt = parent->getComponent<Transform>();

                Vector3f pos = t->position + pt->position;
                Vector3f rot = t->rotation + pt->rotation;
                Vector3f scale = t->scale * pt->scale;
                out.write((char *)&pos, sizeof(Vector3f));
                out.write((char *)&rot, sizeof(Vector3f));
                out.write((char *)&scale, sizeof(Vector3f));
            }
            else {
                out.write((char *)&t->position, sizeof(Vector3f));
                out.write((char *)&t->rotation, sizeof(Vector3f));
                out.write((char *)&t->scale, sizeof(Vector3f));
            }
        }
        if (e->hasComponent<Mesh>()) {
            out.write("m", sizeof(char));
            Mesh* mesh = e->getComponent<Mesh>();

            const uint32_t numVertices = (uint32_t)mesh->vertices.size();
            const uint32_t numTexCoords = (uint32_t)mesh->texCoords.size();
            const uint32_t numNormals = (uint32_t)mesh->normals.size();
            const uint32_t numIndices = (uint32_t)mesh->indices.size();

            out.write((char *)&numVertices, sizeof(numVertices));
            out.write((char *)&mesh->vertices[0], numVertices * sizeof(Vector3f));

            out.write((char *)&numTexCoords, sizeof(numTexCoords));
            out.write((char *)&mesh->texCoords[0], numTexCoords * sizeof(Vector2f));

            out.write((char *)&numNormals, sizeof(numNormals));
            out.write((char *)&mesh->normals[0], numNormals * sizeof(Vector3f));

            out.write((char *)&numIndices, sizeof(numIndices));
            out.write((char *)&mesh->indices[0], numIndices * sizeof(unsigned int));
        }
        if (e->hasComponent<Camera>()) {
            out.write("c", sizeof(char));
            Camera* camera = e->getComponent<Camera>();

            const float fieldOfView = camera->getFovy();
            const float aspectRatio = camera->getAspectRatio();
            const float zNear = camera->getZNear();
            const float zFar = camera->getZFar();

            out.write((char *)&fieldOfView, sizeof(float));
            out.write((char *)&aspectRatio, sizeof(float));
            out.write((char *)&zNear, sizeof(float));
            out.write((char *)&zFar, sizeof(float));
        }
        if (e->hasComponent<PointLight>()) {
            out.write("p", sizeof(char));
            PointLight* pointLight = e->getComponent<PointLight>();

            const float energy = pointLight->energy;

            out.write((char *)&energy, sizeof(float));
        }
        if (e->hasComponent<AttachedTo>()) {
            out.write("a", sizeof(char));
            AttachedTo* attachedTo = e->getComponent<AttachedTo>();

            Entity* parent = scene.getEntityById(attachedTo->parentId);
            const uint32_t pid = (uint32_t)parent->getId();
            out.write((char *)&pid, sizeof(pid));
        }
    }
}
