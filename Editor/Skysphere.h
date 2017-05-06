#pragma once
#ifndef SKYSPHERE_H
#define SKYSPHERE_H

#include <string>

using std::string;

class Skysphere {
public:
    Skysphere(string path) {
        this->path = path;
    }

private:
    string path;
};

#endif /* SKYSPHERE_H */
