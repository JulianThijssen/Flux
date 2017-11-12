#include "SceneConverter.h"

#include "SceneDesc.h"
#include "Skybox.h"
#include "Skysphere.h"

#include "Path.h"

#include "AssetManager.h"
#include "Transform.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "AttachedTo.h"

#include "PointLight.h"
#include "DirectionalLight.h"

#include <fstream>
#include <iostream> // Temp
#include <ctime>

namespace Flux {
    namespace Editor {
        uint32_t swap(unsigned int i) {
            uint32_t h = (i & 0xFF000000) >> 24;
            uint32_t mh = (i & 0x00FF0000) >> 8;
            uint32_t ml = (i & 0x0000FF00) << 8;
            uint32_t l = (i & 0x000000FF) << 24;

            return l | ml | mh | h;
        }

        void copy(Buffer& buffer, uint32_t i) {
            //uint32_t swapped = swap(i);
            memcpy((void *)&buffer.buf[buffer.pos], &i, sizeof(uint32_t));
            buffer.pos += sizeof(uint32_t);
        }

        void copy(Buffer& buffer, const void* data, unsigned int size) {
            memcpy((void *)&buffer.buf[buffer.pos], data, size);
            buffer.pos += size;
        }

        void SceneConverter::convert(const SceneDesc& scene, Path outputPath) {
            clock_t startTime = clock();

            Buffer buffer;
            buffer.buf = new char[1000000000];

            if (scene.skybox) {
                writeSkybox(scene.skybox, buffer);
            }
            else if (scene.skysphere) {
                writeSkysphere(scene.skysphere, buffer);
            }

            const uint32_t numMaterials = (uint32_t)scene.materials.size();
            copy(buffer, numMaterials);
            for (uint32_t i = 0; i < numMaterials; ++i) {
                writeMaterial(i, scene.materials[i], buffer);
            }

            clock_t entityTime = clock();
            const uint32_t numEntities = (uint32_t)scene.entities.size();
            copy(buffer, numEntities);
            for (Entity* e : scene.entities) {
                writeEntity(scene, e, buffer);
            }

            FILE* outFile;
            outFile = fopen(outputPath.c_str(), "wb");
            fwrite(buffer.buf, 1, buffer.pos * sizeof(char), outFile);
            fclose(outFile);

            delete buffer.buf;

            std::cout << "Final buffer position: " << buffer.pos << std::endl;
            clock_t endTime = clock();
            double elapsed = double(endTime - startTime) / CLOCKS_PER_SEC;
            double entityElapsed = double(endTime - entityTime) / CLOCKS_PER_SEC;
            std::cout << "Converting scene took: " << elapsed << " " << entityElapsed << " seconds." << std::endl;
        }

        void SceneConverter::writeSkybox(Editor::Skybox* skybox, Buffer& buffer) {
            const uint32_t type = 0;

            copy(buffer, type);

            const std::string* paths = skybox->getPaths();

            for (int i = 0; i < 6; i++) {
                const uint32_t pathLen = (uint32_t)paths[i].length();
                copy(buffer, pathLen);
                const char* path = paths[i].c_str();
                copy(buffer, path, sizeof(char) * pathLen);
            }
        }

        void SceneConverter::writeSkysphere(Skysphere* skysphere, Buffer& buffer) {
            const uint32_t type = 1;
            std::cout << "SKYSPHERE" << std::endl;
            copy(buffer, type);
            const std::string path = skysphere->getPath();
            const uint32_t pathLen = (uint32_t)path.length();
            copy(buffer, pathLen);
            const char* cpath = path.c_str();
            copy(buffer, cpath, sizeof(char) * pathLen);
        }

        void SceneConverter::writeMaterial(const uint32_t id, MaterialDesc* material, Buffer& buffer) {
            std::cout << "Writing material" << std::endl;
            copy(buffer, id);

            const uint32_t pathLen = (uint32_t)material->path.length();
            copy(buffer, pathLen);

            const char* path = material->path.c_str();
            copy(buffer, path, sizeof(char) * pathLen);
        }

        void SceneConverter::writeEntity(const SceneDesc& scene, Entity* e, Buffer& buffer) {
            const uint32_t id = (uint32_t)e->getId();
            copy(buffer, id);

            const uint32_t numComponents = (uint32_t)e->numComponents();
            copy(buffer, numComponents);

            if (e->hasComponent<Transform>()) {
                copy(buffer, "t", sizeof(char));
                Transform* t = e->getComponent<Transform>();

                copy(buffer, &t->position, sizeof(Vector3f));
                copy(buffer, &t->rotation, sizeof(Vector3f));
                copy(buffer, &t->scale, sizeof(Vector3f));
            }
            if (e->hasComponent<Mesh>()) {
                clock_t meshTime = clock();

                copy(buffer, "m", sizeof(char));
                Mesh* mesh = e->getComponent<Mesh>();

                const uint32_t numVertices = (uint32_t)mesh->vertices.size();
                const uint32_t numTexCoords = (uint32_t)mesh->texCoords.size();
                const uint32_t numNormals = (uint32_t)mesh->normals.size();
                const uint32_t numTangents = (uint32_t)mesh->tangents.size();
                const uint32_t numIndices = (uint32_t)mesh->indices.size();

                copy(buffer, numVertices);
                copy(buffer, &mesh->vertices[0], numVertices * sizeof(Vector3f));

                copy(buffer, numTexCoords);
                copy(buffer, &mesh->texCoords[0], numTexCoords * sizeof(Vector2f));

                copy(buffer, numNormals);
                copy(buffer, &mesh->normals[0], numNormals * sizeof(Vector3f));

                copy(buffer, numTangents);
                copy(buffer, &mesh->tangents[0], numTangents * sizeof(Vector3f));

                copy(buffer, numIndices);
                copy(buffer, &mesh->indices[0], numIndices * sizeof(unsigned int));

                clock_t endMeshTime = clock();
                double elapsed = double(endMeshTime - meshTime) / CLOCKS_PER_SEC;
                std::cout << "Writing mesh took: " << elapsed << " seconds." << std::endl;
            }
            if (e->hasComponent<MeshRenderer>()) {
                copy(buffer, "r", sizeof(char));
                MeshRenderer* mr = e->getComponent<MeshRenderer>();

                copy(buffer, mr->materialID);
            }
            if (e->hasComponent<Camera>()) {
                copy(buffer, "c", sizeof(char));
                Camera* camera = e->getComponent<Camera>();

                const bool perspective = camera->isPerspective();
                copy(buffer, &perspective, sizeof(bool));

                if (perspective) {
                    const float fieldOfView = camera->getFovy();
                    const float aspectRatio = camera->getAspectRatio();
                    const float zNear = camera->getZNear();
                    const float zFar = camera->getZFar();

                    copy(buffer, &fieldOfView, sizeof(float));
                    copy(buffer, &aspectRatio, sizeof(float));
                    copy(buffer, &zNear, sizeof(float));
                    copy(buffer, &zFar, sizeof(float));
                }
                else {
                    const float left = camera->getLeft();
                    const float right = camera->getRight();
                    const float bottom = camera->getBottom();
                    const float top = camera->getTop();
                    const float zNear = camera->getZNear();
                    const float zFar = camera->getZFar();

                    copy(buffer, &left, sizeof(float));
                    copy(buffer, &right, sizeof(float));
                    copy(buffer, &bottom, sizeof(float));
                    copy(buffer, &top, sizeof(float));
                    copy(buffer, &zNear, sizeof(float));
                    copy(buffer, &zFar, sizeof(float));
                }
            }
            if (e->hasComponent<PointLight>()) {
                copy(buffer, "p", sizeof(char));
                PointLight* pointLight = e->getComponent<PointLight>();

                copy(buffer, &pointLight->color, sizeof(Vector3f));
                const float energy = pointLight->energy;
                copy(buffer, &energy, sizeof(float));
            }
            if (e->hasComponent<DirectionalLight>()) {
                copy(buffer, "d", sizeof(char));
                DirectionalLight* dirLight = e->getComponent<DirectionalLight>();

                copy(buffer, &dirLight->direction, sizeof(Vector3f));
                copy(buffer, &dirLight->color, sizeof(Vector3f));

                const float energy = dirLight->energy;
                copy(buffer, &energy, sizeof(float));
            }
            if (e->hasComponent<AttachedTo>()) {
                copy(buffer, "a", sizeof(char));
                AttachedTo* attachedTo = e->getComponent<AttachedTo>();

                Entity* parent = scene.getEntityById(attachedTo->parentId);
                const uint32_t pid = (uint32_t)parent->getId();
                copy(buffer, pid);
            }
        }
    }
}
