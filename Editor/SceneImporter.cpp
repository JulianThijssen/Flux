#include "SceneImporter.h"
#include "ModelImporter.h"
#include "Model.h"
#include "SceneDesc.h"
#include "MaterialDesc.h"

#include <Engine/Source/Entity.h>
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

using json = nlohmann::json;

namespace Flux {
    void SceneImporter::loadScene(const Path path, SceneDesc& scene) {
        String contents = File::loadFile(path.str().c_str());

        const char* cont = contents.c_str();

        json j3 = json::parse(cont);

        for (json& element : j3["materials"]) {
            std::string name = element["id"].get<std::string>();
            std::string path = element["path"].get<std::string>();

            MaterialDesc* material = new MaterialDesc(name, path);
            scene.addMaterial(material);
        }
        for (json& element : j3["entities"]) {
            //std::cout << element.dump() << std::endl;

            Entity* e = new Entity();

            std::string name = element["name"].get<std::string>();
            e->name = name;

            MeshRenderer* meshRenderer = nullptr;
            for (json::iterator it = element["components"][0].begin(); it != element["components"][0].end(); ++it) {
                std::cout << it.key() << " : " << it.value() << "\n";

                if (it.key() == "material") {
                    std::string name = it.value().get<std::string>();

                    int id = 0;
                    for (int i = 0; i < scene.materials.size(); i++) {
                        MaterialDesc* desc = scene.materials[i];

                        if (name == desc->name) {
                            id = i;
                            break;
                        }
                    }

                    meshRenderer = new MeshRenderer();
                    meshRenderer->materialID = id;
                }
                if (it.key() == "model") {
                    std::string path = it.value()["path"].get<std::string>();

                    Model* model = ModelImporter::loadModel(Path(path));

                    for (int i = 0; i < model->meshes.size(); i++) {
                        Entity* child = new Entity();

                        Transform* childT = new Transform();
                        child->addComponent(childT);

                        AttachedTo* attached = new AttachedTo(e->getId());
                        child->addComponent(attached);

                        Mesh* mesh = &model->meshes[i];
                        child->addComponent(mesh);

                        if (meshRenderer != nullptr) {
                            child->addComponent(meshRenderer);
                        }

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
            }
            scene.addEntity(e);
        }

        Entity* mainCamera = new Entity();
        Transform* camT = new Transform();
        camT->position.set(4, 4, 12);
        camT->rotation.set(0, 0, 0);
        mainCamera->addComponent(camT);
        mainCamera->addComponent(new Camera(60, 1024.f/768, 0.1f, 100.f));
        scene.entities.push_back(mainCamera);

        Entity* light = new Entity();
        DirectionalLight* dirLight = new DirectionalLight();
        light->addComponent(dirLight);
        Transform* transform = new Transform();
        transform->position.set(5, 4, 10);
        light->addComponent(transform);
        scene.entities.push_back(light);
    }
}
