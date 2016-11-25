#ifndef FACE_H
#define FACE_H

#include <vector>

namespace Flux {
    class Face {
    public:
        int vi[3];
        int ti[3];
        int ni[3];

        Face() {}
        Face(int vi[3]) {}
        Face(int vi[3], int ti[3], int ni[3]) {}
    };
}

#endif /* FACE_H */
