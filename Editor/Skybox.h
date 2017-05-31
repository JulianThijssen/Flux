#pragma once
#ifndef EDITOR_SKYBOX_H
#define EDITOR_SKYBOX_H

#include <string>

using std::string;

namespace Flux {
    namespace Editor {
        class Skybox {
        public:
            Skybox()
                : paths{ "", "", "", "", "", "" } { }

            Skybox(string right, string left, string top, string bottom, string front, string back) {
                paths[0] = right;
                paths[1] = left;
                paths[2] = top;
                paths[3] = bottom;
                paths[4] = front;
                paths[5] = back;
            }

            void setRight(string path) {
                paths[0] = path;
            }

            void setLeft(string path) {
                paths[1] = path;
            }

            void setTop(string path) {
                paths[2] = path;
            }

            void setBottom(string path) {
                paths[3] = path;
            }

            void setFront(string path) {
                paths[4] = path;
            }

            void setBack(string path) {
                paths[5] = path;
            }

            const string* getPaths() const {
                return paths;
            }
        private:
            string paths[6];
        };
    }
}

#endif /* EDITOR_SKYBOX_H */
