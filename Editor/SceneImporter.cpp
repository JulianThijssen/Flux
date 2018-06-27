#include "SceneImporter.h"

#include "ModelImporter.h"
#include "Model.h"
#include "SceneDesc.h"
#include "MaterialDesc.h"
#include "Skybox.h"
#include "Skysphere.h"

#include "Entity.h"
#include "AttachedTo.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "AreaLight.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "Transform.h"

#include "Util/File.h"
#include "Path.h"

#include "json.hpp"
#include <fstream>
#include <string>
#include <iostream>
#include <ctime>

using json = nlohmann::json;

namespace Flux {
    namespace Editor {
        SceneImporter::Status SceneImporter::loadScene(const Path path, SceneDesc& scene) {
            clock_t clockStart, clockEnd, clockMid;
            clockStart = clock();
            double elapsed;

            ModelImporter modelImporter;

            String contents;
            try {
                contents = File::loadFile(path.str().c_str());
            }
            catch (const std::invalid_argument& e) {
                return Status::FileNotFound;
            }

            const char* cont = contents.c_str();

            json j3 = json::parse(cont);

            if (j3.find("sun") != j3.end()) {
                json& element = j3["sun"];

                json& rad = element["radiance"];
                json& dir = element["rotation"];
                
                Entity* light = new Entity();
                DirectionalLight* dirLight = new DirectionalLight();
                dirLight->color.set(rad[0], rad[1], rad[2]);
                light->addComponent(dirLight);
                light->addComponent(new Camera(-40, 40, -40, 40, -50, 50));
                Transform* t1 = new Transform();
                t1->rotation.set(dir[0], dir[1], dir[2]);
                light->addComponent(t1);
                scene.addEntity(light);
            }
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

                        // For every material find the material ID corresponding to its name
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

                            // Save the material ID in a mesh renderer
                            MeshRenderer* meshRenderer = new MeshRenderer();
                            meshRenderer->materialID = id;
                            meshRenderers.push_back(meshRenderer);
                        }
                    }
                    if (it.key() == "model") {
                        clock_t clockModel = clock();

                        std::string path = it.value()["path"].get<std::string>();

                        Model model = modelImporter.ImportFromFile(Path(path));

                        for (int i = 0; i < model.meshes.size(); i++) {
                            Entity* child = new Entity();
                            std::cout << "Loading child with id: " << child->getId() << std::endl;
                            Transform* childT = new Transform();
                            child->addComponent(childT);

                            AttachedTo* attached = new AttachedTo(e->getId());
                            child->addComponent(attached);

                            std::unique_ptr<Mesh>& mesh = model.meshes[i];
                            child->addComponent(mesh.release());

                            if (i < meshRenderers.size()) {
                                child->addComponent(meshRenderers[i]);
                            }

                            scene.addEntity(child);
                        }

                        clockEnd = clock();
                        elapsed = double(clockEnd - clockModel) / CLOCKS_PER_SEC;
                        std::cout << "Importing models took: " << elapsed << " seconds." << std::endl;
                    }
                    if (it.key() == "camera") {
                        json c = it.value();

                        float fov = 60;
                        float aspect = 1;
                        float zNear = 0.1f;
                        float zFar = 400;

                        for (json::iterator it = c.begin(); it != c.end(); ++it) {
                            if (it.key() == "fov")
                                fov = c[it.key()];
                            if (it.key() == "aspect")
                                aspect = c[it.key()];
                            if (it.key() == "zNear")
                                zNear = c[it.key()];
                            if (it.key() == "zFar")
                                zFar = c[it.key()];
                        }

                        e->addComponent(new Camera(fov, aspect, zNear, zFar));
                    }
                    if (it.key() == "arealight") {
                        AreaLight* areaLight = new AreaLight();
                        areaLight->color.set(20, 10, 1);
                        e->addComponent(areaLight);
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

            //Entity* light2 = new Entity();
            //PointLight* pointLight = new PointLight();
            //pointLight->color.set(0, 8, 0.6);
            //light2->addComponent(pointLight);
            //Transform* t2 = new Transform();
            //t2->position.set(0, 1, 0);
            //light2->addComponent(t2);
            //scene.addEntity(light2);

            clockEnd = clock();
            elapsed = double(clockEnd - clockStart) / CLOCKS_PER_SEC;

            std::cout << "Scene importing took: " << elapsed << " seconds." << std::endl;

            return Status::Success;
        }
    }
}
