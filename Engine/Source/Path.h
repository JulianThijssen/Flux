/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** Path.h
** Declares a class that makes common file operations easier.
**
** Author: Julian Thijssen
** -------------------------------------------------------------------------*/

#pragma once
#ifndef PATH_H
#define PATH_H

#include "String.h"
#include <string>

namespace Flux {
    class Path {
    public:
        Path(String path);
        Path(std::string path);

        Path getCurrentFolder();
        std::string getExtension();
        void correctSlashes();

        std::string str() const;

        /* Operator overloads */
        bool operator==(const Path& path) const;
        bool operator!=(const Path& path) const;
    private:
        std::string string;
    };
}

#endif /* PATH_H */
