#pragma once

#include <string>

using std::string;

namespace Flux {
    class Skysphere {
    public:
        Skysphere(string path) {
            this->path = path;
        }

        const string getPath() const {
            return path;
        }
    private:
        string path;
    };
}
