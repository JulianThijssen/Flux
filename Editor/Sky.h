
#include <vector>
#include <string>

using std::string;

class Sky {
    Sky(string front, string back, string left, string right, string top, string bottom) {
        paths.push_back(front);
        paths.push_back(back);
        paths.push_back(left);
        paths.push_back(right);
        paths.push_back(top);
        paths.push_back(bottom);
        this->isSkybox = true;
    }

    Sky(string skysphere) {
        paths.push_back(skysphere);
        this->isSkybox = false;
    }

    bool isSkybox() { return isSkybox; }
private:
    bool isSkybox;

    std::vector<std::string> paths;
};
