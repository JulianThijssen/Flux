#include "SceneConverter.h"

#include "SceneDesc.h"
#include "Skybox.h"
#include "Skysphere.h"

#include "../Engine/Source/Path.h"

#include <Engine/Source/AssetManager.h>
#include <Engine/Source/Transform.h>
#include <Engine/Source/Mesh.h>
#include <Engine/Source/MeshRenderer.h>
#include <Engine/Source/AttachedTo.h>

#include <Engine/Source/PointLight.h>
#include <Engine/Source/DirectionalLight.h>

#include <fstream>
#include <iostream> // Temp

namespace Flux {
    void SceneConverter::convert(const SceneDesc& scene, Path outputPath) {
        std::ofstream outFile;
        outFile.open(outputPath.str().c_str(), ios::out | ios::binary);

        if (scene.skybox) {
            writeSkybox(scene.skybox, outFile);
        }
        else if (scene.skysphere) {
            writeSkysphere(scene.skysphere, outFile);
        }

        const uint32_t numMaterials = (uint32_t) scene.materials.size();
        outFile.write((char *)&numMaterials, sizeof(numMaterials));
        for (uint32_t i = 0; i < numMaterials; ++i) {
            writeMaterial(i, scene.materials[i], outFile);
        }

        const uint32_t numEntities = (uint32_t) scene.entities.size();
        outFile.write((char *) &numEntities, sizeof(numEntities));
        for (Entity* e : scene.entities) {
            writeEntity(scene, e, outFile);
        }

        outFile.close();
    }

    void SceneConverter::writeSkybox(Skybox* skybox, std::ofstream& out) {
        const uint32_t type = 0;
        out.write((char *)&type, sizeof(type));
        const std::string* paths = skybox->getPaths();

        for (int i = 0; i < 6; i++) {
            const uint32_t pathLen = (uint32_t)paths[0].length();
            out.write((char *)&pathLen, sizeof(pathLen));
            out.write(paths[0].c_str(), sizeof(char) * paths[0].length());
        }
    }

    void SceneConverter::writeSkysphere(Skysphere* skysphere, std::ofstream& out) {
        const uint32_t type = 1;
        out.write((char *)&type, sizeof(type));

        const std::string path = skysphere->getPath();
        const uint32_t pathLen = (uint32_t)path.length();
        out.write((char *)&pathLen, sizeof(pathLen));
        out.write(path.c_str(), sizeof(char) * path.length());
    }

    void SceneConverter::writeMaterial(const uint32_t id, MaterialDesc* material, std::ofstream& out) {
        out.write((char *)&id, sizeof(id));

        const uint32_t pathLen = (uint32_t) material->path.length();
        out.write((char *)&pathLen, sizeof(pathLen));

        out.write(material->path.c_str(), sizeof(char) * pathLen);
    }

    void SceneConverter::writeEntity(const SceneDesc& scene, Entity* e, std::ofstream& out) {
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
            const uint32_t numTangents = (uint32_t)mesh->tangents.size();
            const uint32_t numIndices = (uint32_t)mesh->indices.size();

            out.write((char *)&numVertices, sizeof(numVertices));
            out.write((char *)&mesh->vertices[0], numVertices * sizeof(Vector3f));

            out.write((char *)&numTexCoords, sizeof(numTexCoords));
            out.write((char *)&mesh->texCoords[0], numTexCoords * sizeof(Vector2f));

            out.write((char *)&numNormals, sizeof(numNormals));
            out.write((char *)&mesh->normals[0], numNormals * sizeof(Vector3f));

            out.write((char *)&numTangents, sizeof(numTangents));
            out.write((char *)&mesh->tangents[0], numTangents * sizeof(Vector3f));

            out.write((char *)&numIndices, sizeof(numIndices));
            out.write((char *)&mesh->indices[0], numIndices * sizeof(unsigned int));
        }
        if (e->hasComponent<MeshRenderer>()) {
            out.write("r", sizeof(char));
            MeshRenderer* mr = e->getComponent<MeshRenderer>();

            out.write((char *)&mr->materialID, sizeof(uint32_t));
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

            out.write((char *)&pointLight->color, sizeof(Vector3f));
            const float energy = pointLight->energy;
            out.write((char *)&energy, sizeof(float));
        }
        if (e->hasComponent<DirectionalLight>()) {
            out.write("d", sizeof(char));
            DirectionalLight* dirLight = e->getComponent<DirectionalLight>();

            out.write((char *)&dirLight->direction, sizeof(Vector3f));
            out.write((char *)&dirLight->color, sizeof(Vector3f));

            const float energy = dirLight->energy;
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
