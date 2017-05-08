#pragma once
#ifndef SKYSPHERE_H
#define SKYSPHERE_H

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

#endif /* SKYSPHERE_H */
