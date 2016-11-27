#include "SceneLoader.h"

#include "Scene.h"
#include "Entity.h"
#include "Model.h"
#include "ModelLoader.h"
#include "Path.h"

#include "json.hpp"
#include <fstream>
#include <string>
#include <iostream>

using json = nlohmann::json;

namespace Flux {
    void SceneLoader::loadScene(Scene& scene) {
        std::string contents;
        std::string line;
        std::ifstream myfile("res/Indoors.json");
        if (myfile.is_open())
        {
            while (getline(myfile, line))
            {
                contents += line + '\n';
            }
            myfile.close();
        }

        const char* cont = contents.c_str();
        json j3 = json::parse(cont);

        //for (json::iterator it = j3.begin(); it != j3.end(); ++it) {
        //    std::cout << it.key() << " : " << it.value() << "\n";
        //}

        for (json& element : j3) {
            //std::cout << element["name"] << std::endl;
            //std::cout << element["components"] << std::endl;

            Entity* e = new Entity();

            for (json::iterator it = element["components"][0].begin(); it != element["components"][0].end(); ++it) {
                std::cout << it.key() << " : " << it.value() << "\n";
                
                if (it.key() == "model") {
                    std::string pathString = it.value()["path"].dump();
                    pathString = pathString.substr(1, pathString.size() - 2);

                    Path path(pathString);
                    Model* model = ModelLoader::loadModel(path);
                    e->addComponent(model);
                }
                if (it.key() == "transform") {
                    Transform* transform = new Transform();

                    json t = it.value();
                    for (json::iterator it = t.begin(); it != t.end(); ++it) {
                        //std::cout << it.key() << std::endl;
                        std::cout << t[it.key()] << std::endl;
                        float x = t[it.key()][0];
                        float y = t[it.key()][1];
                        float z = t[it.key()][2];
                        
                        std::cout << x << y << z << std::endl;
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
    }
}
