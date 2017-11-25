#include "SceneLoader.h"
#include "MaterialLoader.h"
#include "TextureLoader.h"

#include "Path.h"
#include "Scene.h"
#include "Skybox.h"

#include "Transform.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "AttachedTo.h"

#include <fstream>
#include <iostream> // Temp

#include <glad/glad.h>

namespace Flux {
    // TODO Move mesh loading and uploading to a separate class
    void uploadMesh(Mesh* mesh) {
        // Generate vertex array object
        glGenVertexArrays(1, &mesh->handle);
        glBindVertexArray(mesh->handle);

        // Store faces in a buffer
        glGenBuffers(1, &mesh->indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(unsigned int), &mesh->indices[0], GL_STATIC_DRAW);

        // Store vertices in a buffer
        GLuint vertexVBO;
        glGenBuffers(1, &vertexVBO);
        glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
        glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(Vector3f), &mesh->vertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
        glEnableVertexAttribArray(0);

        // Store texture coordinates in a buffer
        GLuint texCoordVBO;
        glGenBuffers(1, &texCoordVBO);
        glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
        glBufferData(GL_ARRAY_BUFFER, mesh->texCoords.size() * sizeof(Vector2f), &mesh->texCoords[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
        glEnableVertexAttribArray(1);

        // Store normals in a buffer
        GLuint normalVBO;
        glGenBuffers(1, &normalVBO);
        glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
        glBufferData(GL_ARRAY_BUFFER, mesh->normals.size() * sizeof(Vector3f), &mesh->normals[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, 0);
        glEnableVertexAttribArray(2);

        // Store tangents in a buffer
        GLuint tangentVBO;
        glGenBuffers(1, &tangentVBO);
        glBindBuffer(GL_ARRAY_BUFFER, tangentVBO);
        glBufferData(GL_ARRAY_BUFFER, mesh->tangents.size() * sizeof(Vector3f), &mesh->tangents[0], GL_STATIC_DRAW);
        glVertexAttribPointer(3, 3, GL_FLOAT, false, 0, 0);
        glEnableVertexAttribArray(3);

        // Unbind the buffers
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    bool SceneLoader::loadScene(const Path path, Scene& scene) {
        std::cout << "LOADING SCENE" << std::endl;
        std::ifstream inFile;

        inFile.open(path.str().c_str(), ios::in | ios::binary);

        if (inFile.fail()) {
            std::cout << "Failed to load file: " << path.str() << std::endl;
            return false;
        }

        unsigned int skyType;
        inFile.read((char *)&skyType, sizeof(skyType));
        std::cout << "Sky type: " << skyType << std::endl;
        if (skyType == 0) {
            char* paths[6];
            for (int i = 0; i < 6; i++) {
                uint32_t numChars;
                inFile.read((char *)&numChars, sizeof(numChars));

                paths[i] = new char[numChars + 1];
                paths[i][numChars] = 0;
                inFile.read(paths[i], numChars * sizeof(char));
                std::cout << paths[i] << std::endl;
            }
            scene.skybox = new Skybox(paths);
        }
        if (skyType == 1) {
            uint32_t numChars;
            inFile.read((char *)&numChars, sizeof(numChars));
            std::cout << "Sky num chars: " << numChars << std::endl;
            char* path = new char[numChars + 1];
            path[numChars] = 0;
            inFile.read(path, numChars * sizeof(char));
            std::cout << "Reading skysphere: " << path << std::endl;
            scene.skySphere = TextureLoader::loadTexture(Path(path), HDR, CLAMP, SamplingConfig(NEAREST, NEAREST, NONE));
            delete path;
        }

        unsigned int numMaterials;
        inFile.read((char *) &numMaterials, sizeof(numMaterials));
        std::cout << "NUM MATERIALS: " << numMaterials << std::endl;

        for (unsigned int i = 0; i < numMaterials; ++i) {
            uint32_t id;
            inFile.read((char *)&id, sizeof(id));

            uint32_t numChars;
            inFile.read((char *)&numChars, sizeof(numChars));

            char* path = new char[numChars+1];
            path[numChars] = 0;
            inFile.read(path, numChars * sizeof(char));

            Material* mat = MaterialLoader::loadMaterial(Path(path));
            scene.addMaterial(mat);
            delete path;
        }

        unsigned int numEntities;
        inFile.read((char *) &numEntities, sizeof(numEntities));
        std::cout << "NUM ENTITIES: " << numEntities << std::endl;

        for (unsigned int i = 0; i < numEntities; ++i) {
            Entity* e = new Entity();

            uint32_t id;
            inFile.read((char *) &id, sizeof(id));
            e->setId(id);
            std::cout << "Loading entity with id: " << e->getId() << std::endl;
            unsigned int numComponents;
            inFile.read((char *) &numComponents, sizeof(numComponents));
            
            char component;
            for (unsigned int j = 0; j < numComponents; j++) {
                inFile.read(&component, sizeof(char));

                if (component == 't') {
                    Transform* t = new Transform();
                    inFile.read((char *)&t->position, sizeof(Vector3f));
                    std::cout << "Position: " << t->position.str() << std::endl;
                    inFile.read((char *)&t->rotation, sizeof(Vector3f));
                    inFile.read((char *)&t->scale, sizeof(Vector3f));
                    e->addComponent(t);
                }
                if (component == 'm') {
                    Mesh* mesh = new Mesh();

                    unsigned int numVertices;
                    inFile.read((char *) &numVertices, sizeof(numVertices));
                    mesh->vertices.resize(numVertices);
                    inFile.read((char *) &mesh->vertices[0], numVertices * sizeof(Vector3f));

                    unsigned int numTexCoords;
                    inFile.read((char *) &numTexCoords, sizeof(numTexCoords));
                    mesh->texCoords.resize(numTexCoords);
                    inFile.read((char *) &mesh->texCoords[0], numTexCoords * sizeof(Vector2f));

                    unsigned int numNormals;
                    inFile.read((char *) &numNormals, sizeof(numNormals));
                    mesh->normals.resize(numNormals);
                    inFile.read((char *) &mesh->normals[0], numNormals * sizeof(Vector3f));

                    unsigned int numTangents;
                    inFile.read((char *)&numTangents, sizeof(numTangents));
                    mesh->tangents.resize(numTangents);
                    inFile.read((char *)&mesh->tangents[0], numTangents * sizeof(Vector3f));

                    unsigned int numIndices;
                    inFile.read((char *) &numIndices, sizeof(numIndices));
                    mesh->indices.resize(numIndices);
                    inFile.read((char *) &mesh->indices[0], numIndices * sizeof(unsigned int));

                    uploadMesh(mesh);

                    e->addComponent(mesh);
                }
                if (component == 'r') {
                    uint32_t id;
                    inFile.read((char *)&id, sizeof(id));

                    MeshRenderer* mr = new MeshRenderer();
                    mr->materialID = id;

                    e->addComponent(mr);
                }
                if (component == 'c') {
                    bool perspective = true;
                    inFile.read((char *)&perspective, sizeof(bool));
                    Camera* camera = nullptr;

                    if (perspective) {
                        float fovy, aspect, zNear, zFar;

                        inFile.read((char *)&fovy, sizeof(fovy));
                        inFile.read((char *)&aspect, sizeof(aspect));
                        inFile.read((char *)&zNear, sizeof(zNear));
                        inFile.read((char *)&zFar, sizeof(zFar));
                        std::cout << fovy << " " << aspect << " " << zNear << " " << zFar << std::endl;
                        camera = new Camera(fovy, aspect, zNear, zFar);
                    }
                    else {
                        float left, right, bottom, top, zNear, zFar;

                        inFile.read((char *)&left, sizeof(left));
                        inFile.read((char *)&right, sizeof(right));
                        inFile.read((char *)&bottom, sizeof(bottom));
                        inFile.read((char *)&top, sizeof(top));
                        inFile.read((char *)&zNear, sizeof(zNear));
                        inFile.read((char *)&zFar, sizeof(zFar));
                        std::cout << left << " " << right << " " << bottom << " " << top << " " << zNear << " " << zFar << std::endl;
                        camera = new Camera(left, right, bottom, top, zNear, zFar);
                    }

                    e->addComponent(camera);
                }
                if (component == 'p') {
                    PointLight* pointLight = new PointLight();

                    inFile.read((char *)&pointLight->color, sizeof(Vector3f));
                    float energy;
                    inFile.read((char *) &energy, sizeof(energy));

                    pointLight->energy = energy;

                    e->addComponent(pointLight);
                }
                if (component == 'd') {
                    DirectionalLight* dirLight = new DirectionalLight();

                    inFile.read((char *)&dirLight->direction, sizeof(Vector3f));
                    inFile.read((char *)&dirLight->color, sizeof(Vector3f));
                    inFile.read((char *)&dirLight->energy, sizeof(float));
                    
                    e->addComponent(dirLight);
                }
                if (component == 'a') {
                    uint32_t pid;
                    inFile.read((char *) &pid, sizeof(pid));
                    std::cout << "Attached to: " << pid << std::endl;
                    AttachedTo* attachedTo = new AttachedTo(pid);

                    e->addComponent(attachedTo);
                }
            }

            // TODO add main camera id to the scene description
            if (e->hasComponent<PointLight>() || e->hasComponent<DirectionalLight>()) {
                scene.lights.push_back(e);
            }
            else if (e->hasComponent<Camera>()) {
                std::cout << "SETTING MAIN CAMERA" << std::endl;
                scene.mainCamera = e;
            }
            else {
                scene.addEntity(e);
            }
        }

        //Transform* camT = new Transform();
        //camT->position.set(0, 4, 15);
        //camT->rotation.set(0, 0, 0);
        //scene.mainCamera.addComponent(camT);
        //scene.mainCamera.addComponent(new Camera(60, 1024.f / 768, 0.1f, 100.f));

        //Entity* light = new Entity();
        //PointLight* point = new PointLight();
        //light->addComponent(point);
        //Transform* transform = new Transform();
        //transform->position.set(5, 2, 5);
        //light->addComponent(transform);
        //scene.lights.push_back(light);

        return true;
    }
}
