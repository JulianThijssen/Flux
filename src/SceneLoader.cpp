#include "SceneLoader.h"

#include "Scene.h"
#include "Entity.h"
#include "Model.h"
#include "AttachedTo.h"
#include "MeshRenderer.h"
#include "Path.h"
#include "File.h"

#include "ModelLoader.h"
#include "MaterialLoader.h"
#include "TextureLoader.h"
#include "AssetManager.h"

#include "json.hpp"
#include <fstream>
#include <string>
#include <iostream>

using json = nlohmann::json;

namespace Flux {
    void SceneLoader::loadScene(Scene& scene) {
        String contents = File::loadFile("res/Indoors.json");

        const char* cont = contents.c_str();

        json j3 = json::parse(cont);

        for (json& element : j3["materials"]) {
            std::string id = element["id"].get<std::string>();
            std::string path = element["path"].get<std::string>();

            Material* material = MaterialLoader::loadMaterial(Path(path));
            AssetManager::addMaterial(id, material);
        }
        for (json& element : j3["entities"]) {
            std::cout << element.dump() << std::endl;

            Entity* e = new Entity();

            for (json::iterator it = element["components"][0].begin(); it != element["components"][0].end(); ++it) {
                std::cout << it.key() << " : " << it.value() << "\n";
                
                if (it.key() == "model") {
                    std::string path = it.value()["path"].get<std::string>();

                    Model* model = ModelLoader::loadModel(Path(path));

                    for (int i = 0; i < model->meshes.size(); i++) {
                        Entity* child = new Entity();

                        Transform* childT = new Transform();
                        child->addComponent(childT);

                        AttachedTo* attached = new AttachedTo(e);
                        child->addComponent(attached);

                        child->addComponent(&model->meshes[i]);
                        scene.addEntity(child);
                    }
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
                if (it.key() == "material") {
                    std::string id = it.value()["id"].get<std::string>();

                    MeshRenderer* meshRenderer = new MeshRenderer();
                    meshRenderer->materialID = id;

                    e->addComponent(meshRenderer);
                }
            }
            scene.addEntity(e);
        }

        Transform* camT = new Transform();
        camT->position.set(0, 5, 15);
        camT->rotation.set(-22.5f, 0, 0);
        scene.mainCamera.addComponent(camT);
        scene.mainCamera.addComponent(new Camera(60, 1, 0.1f, 100.f));

        Entity* light = new Entity();
        PointLight* point = new PointLight();
        light->addComponent(point);
        Transform* transform = new Transform();
        transform->position.set(5, 2, 5);
        light->addComponent(transform);
        scene.lights.push_back(light);
    }
}
