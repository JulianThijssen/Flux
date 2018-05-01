#include "SceneLoader.h"
#include "MaterialLoader.h"

#include "Util/Path.h"
#include "Scene.h"
#include "Skybox.h"

#include "Transform.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "AreaLight.h"
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
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    uint32_t readUnsignedInt(std::ifstream& stream) {
        uint32_t i;
        stream.read((char *) &i, sizeof(i));
        return i;
    }

    bool SceneLoader::loadScene(const Path path, Scene& scene) {
        std::cout << "LOADING SCENE" << std::endl;
        std::ifstream inFile;

        inFile.open(path.str().c_str(), ios::in | ios::binary);

        if (inFile.fail()) {
            std::cout << "Failed to load file: " << path.str() << std::endl;
            return false;
        }

        uint32_t skyType = readUnsignedInt(inFile);
        std::cout << "Sky type: " << skyType << std::endl;
        if (skyType == 1) {
            std::vector<Path> paths;
            for (int i = 0; i < 6; i++) {
                const uint32_t numChars = readUnsignedInt(inFile);

                std::vector<char> p(numChars + 1);
                p[numChars] = 0;
                inFile.read(p.data(), numChars * sizeof(char));
                //std::cout << paths[i].c_str() << std::endl;
                paths.push_back(Path(p.data()));
            }
            scene.skybox = new Skybox(paths);
        }
        if (skyType == 2) {
            uint32_t numChars = readUnsignedInt(inFile);
            std::cout << "Sky num chars: " << numChars << std::endl;
            char* path = new char[numChars + 1];
            path[numChars] = 0;
            inFile.read(path, numChars * sizeof(char));
            std::cout << "Reading skysphere: " << path << std::endl;
            scene.skySphere = new Texture2D();
            scene.skySphere->loadFromFile(Path(path), HDR);
            scene.skySphere->setSampling(LINEAR, LINEAR);
            scene.skySphere->setWrapping(REPEAT, REPEAT);

            delete path;
        }
        
        uint32_t numMaterials = readUnsignedInt(inFile);
        std::cout << "NUM MATERIALS: " << numMaterials << std::endl;

        for (unsigned int i = 0; i < numMaterials; ++i) {
            uint32_t id = readUnsignedInt(inFile);

            uint32_t numChars = readUnsignedInt(inFile);

            char* path = new char[numChars+1];
            path[numChars] = 0;
            inFile.read(path, numChars * sizeof(char));

            Material* mat = MaterialLoader::loadMaterial(Path(path));
            scene.addMaterial(mat);
            delete path;
        }

        uint32_t numEntities = readUnsignedInt(inFile);
        std::cout << "NUM ENTITIES: " << numEntities << std::endl;

        for (unsigned int i = 0; i < numEntities; ++i) {
            Entity* e = new Entity();

            uint32_t id = readUnsignedInt(inFile);
            e->setId(id);
            std::cout << "Loading entity with id: " << e->getId() << std::endl;
            uint32_t numComponents = readUnsignedInt(inFile);
            
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

                    uint32_t numVertices = readUnsignedInt(inFile);
                    mesh->vertices.resize(numVertices);
                    inFile.read((char *) &mesh->vertices[0], numVertices * sizeof(Vector3f));

                    uint32_t numTexCoords = readUnsignedInt(inFile);
                    mesh->texCoords.resize(numTexCoords);
                    inFile.read((char *) &mesh->texCoords[0], numTexCoords * sizeof(Vector2f));

                    uint32_t numNormals = readUnsignedInt(inFile);
                    mesh->normals.resize(numNormals);
                    inFile.read((char *) &mesh->normals[0], numNormals * sizeof(Vector3f));

                    uint32_t numTangents = readUnsignedInt(inFile);
                    mesh->tangents.resize(numTangents);
                    inFile.read((char *)&mesh->tangents[0], numTangents * sizeof(Vector3f));

                    uint32_t numIndices = readUnsignedInt(inFile);
                    mesh->indices.resize(numIndices);
                    inFile.read((char *) &mesh->indices[0], numIndices * sizeof(unsigned int));

                    uploadMesh(mesh);

                    e->addComponent(mesh);
                }
                if (component == 'r') {
                    uint32_t id = readUnsignedInt(inFile);

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
                if (component == 'l') {
                    AreaLight* areaLight = new AreaLight();

                    inFile.read((char *)&areaLight->color, sizeof(Vector3f));
                    float energy;
                    inFile.read((char *)&energy, sizeof(energy));

                    areaLight->energy = energy;

                    e->addComponent(areaLight);
                }
                if (component == 'a') {
                    uint32_t pid = readUnsignedInt(inFile);
                    std::cout << "Attached to: " << pid << std::endl;
                    AttachedTo* attachedTo = new AttachedTo(pid);

                    e->addComponent(attachedTo);
                }
            }

            // TODO add main camera id to the scene description
            if (e->hasComponent<PointLight>() || e->hasComponent<DirectionalLight>() || e->hasComponent<AreaLight>()) {
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
