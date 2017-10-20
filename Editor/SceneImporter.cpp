#include "SceneImporter.h"
#include "ModelImporter.h"
#include "Model.h"
#include "SceneDesc.h"
#include "MaterialDesc.h"
#include "Skybox.h"
#include "Skysphere.h"

#include <Editor/Entity.h>
#include <Engine/Source/AttachedTo.h>
#include <Engine/Source/DirectionalLight.h>
#include <Engine/Source/PointLight.h>
#include <Engine/Source/MeshRenderer.h>
#include <Engine/Source/Path.h>
#include <Engine/Source/File.h>

#include <Engine/Source/TextureLoader.h>

#include "json.hpp"
#include <fstream>
#include <string>
#include <iostream>
#include <ctime>

using json = nlohmann::json;

namespace Flux {
    namespace Editor {
        void SceneImporter::loadScene(const Path path, SceneDesc& scene) {
            clock_t clockStart, clockEnd, clockMid;
            clockStart = clock();
            double elapsed;

            String contents = File::loadFile(path.str().c_str());

            const char* cont = contents.c_str();

            json j3 = json::parse(cont);

            if (j3.find("skysphere") != j3.end()) {
                std::string path = j3["skysphere"].get<std::string>();
                scene.skysphere = new Skysphere(path);
            }
            if (j3.find("skybox") != j3.end()) {
                scene.skybox = new Editor::Skybox();
                for (json& face : j3["skybox"]) {
                    for (auto it = face.begin(); it != face.end(); ++it)
                    {
                        std::string name = it.key();
                        std::string path = it.value().get<std::string>();

                        if (name == "right")
                            scene.skybox->setRight(path);
                        if (name == "left")
                            scene.skybox->setLeft(path);
                        if (name == "top")
                            scene.skybox->setTop(path);
                        if (name == "bottom")
                            scene.skybox->setBottom(path);
                        if (name == "front")
                            scene.skybox->setFront(path);
                        if (name == "back")
                            scene.skybox->setBack(path);
                    }
                }
            }

            for (json& element : j3["materials"]) {
                std::string name = element["id"].get<std::string>();
                std::string path = element["path"].get<std::string>();

                MaterialDesc* material = new MaterialDesc(name, path);
                scene.addMaterial(material);
            }

            clockMid = clock();


            for (json& element : j3["entities"]) {
                //std::cout << element.dump() << std::endl;

                Entity* e = new Entity();
                std::cout << "Loading entity with id: " << e->getId() << std::endl;
                std::string name = element["name"].get<std::string>();
                e->name = name;

                std::vector<MeshRenderer*> meshRenderers;
                for (json::iterator it = element["components"][0].begin(); it != element["components"][0].end(); ++it) {
                    std::cout << "Iterator: " << it.key() << " : " << it.value() << "\n";

                    if (it.key() == "materials") {
                        unsigned int numMaterials = (unsigned int)it.value().size();

                        for (unsigned int i = 0; i < numMaterials; i++) {
                            std::string name = it.value()[i].get<std::string>();

                            int id = 0;
                            for (int mId = 0; mId < scene.materials.size(); mId++) {
                                MaterialDesc* desc = scene.materials[mId];

                                if (name == desc->name) {
                                    id = mId;
                                    break;
                                }
                            }

                            MeshRenderer* meshRenderer = new MeshRenderer();
                            meshRenderer->materialID = id;
                            meshRenderers.push_back(meshRenderer);
                        }
                    }
                    if (it.key() == "model") {
                        clock_t clockModel = clock();

                        std::string path = it.value()["path"].get<std::string>();

                        Model* model = ModelImporter::loadModel(Path(path));

                        for (int i = 0; i < model->meshes.size(); i++) {
                            Entity* child = new Entity();
                            std::cout << "Loading child with id: " << child->getId() << std::endl;
                            Transform* childT = new Transform();
                            child->addComponent(childT);

                            AttachedTo* attached = new AttachedTo(e->getId());
                            child->addComponent(attached);

                            Mesh* mesh = &model->meshes[i];
                            child->addComponent(mesh);

                            if (i < meshRenderers.size()) {
                                child->addComponent(meshRenderers[i]);
                            }

                            scene.addEntity(child);
                        }

                        clockEnd = clock();
                        elapsed = double(clockEnd - clockModel) / CLOCKS_PER_SEC;
                        std::cout << "Importing models took: " << elapsed << " seconds." << std::endl;
                    }
                    if (it.key() == "transform") {
                        Transform* transform = new Transform();

                        json t = it.value();
                        for (json::iterator it = t.begin(); it != t.end(); ++it) {
                            float x = t[it.key()][0];
                            float y = t[it.key()][1];
                            float z = t[it.key()][2];

                            if (it.key() == "position") {
                                transform->position.set(x, y, z);
                            }
                            if (it.key() == "rotation") {
                                transform->rotation.set(x, y, z);
                            }
                            if (it.key() == "scale") {
                                transform->scale.set(x, y, z);
                            }
                        }
                        e->addComponent(transform);
                    }
                }
                scene.addEntity(e);
            }

            ///////////////
            clockEnd = clock();
            elapsed = double(clockEnd - clockMid) / CLOCKS_PER_SEC;
            std::cout << "Parsing entities took: " << elapsed << " seconds." << std::endl;
            clockMid = clock();
            //////////////

            Entity* mainCamera = new Entity();
            Transform* camT = new Transform();
            camT->position.set(-26, 2, 14); //camT->position.set(4, 4, 12);
            camT->rotation.set(0, -90, 0);
            mainCamera->addComponent(camT);
            mainCamera->addComponent(new Camera(60, 1920.0f / 1080, 0.1f, 400.f));
            scene.entities.push_back(mainCamera);

            Entity* light = new Entity();
            DirectionalLight* dirLight = new DirectionalLight();
            dirLight->color.set(10, 4, 1.5f);
            dirLight->direction.set(-0.471409702f, -0.5061866455f, 0.722182783f);
            dirLight->direction.normalise();
            light->addComponent(dirLight);
            light->addComponent(new Camera(-20, 20, -20, 20, -50, 50));
            Transform* t1 = new Transform();
            t1->position.set(-4, 0, 16);
            t1->rotation.set(-30, 160, 0);
            light->addComponent(t1);
            scene.entities.push_back(light);

            //Entity* light2 = new Entity();
            //PointLight* pointLight = new PointLight();
            //pointLight->color.set(0.4, 0.4, 0.6);
            //light2->addComponent(pointLight);
            //Transform* t2 = new Transform();
            //t2->position.set(5, 4, 10);
            //light2->addComponent(t2);
            //scene.entities.push_back(light2);

            clockEnd = clock();
            elapsed = double(clockEnd - clockStart) / CLOCKS_PER_SEC;

            std::cout << "Scene importing took: " << elapsed << " seconds." << std::endl;
        }
    }
}
