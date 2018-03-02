#pragma once
#ifndef FILE_H
#define FILE_H

#include "Log.h"

#include "Util/String.h"

#include <string>
#include <vector>
#include <fstream>

using namespace std;

namespace Flux {
    class File {
    public:
        static vector<String> loadLines(const char* path) {
            ifstream file(path);
            if (file.fail() || !file.is_open()) {
                Log::error("No such file: " + string(path));
            }

            vector<String> lines;
            string line;
            while (getline(file, line)) {
                lines.push_back(String(line));
            }
            return lines;
        }

        static String loadFile(const char* path) {
            ifstream file(path);
            if (file.fail() || !file.is_open()) {
                throw std::invalid_argument("No such file: " + string(path));
            }

            string source;
            string line;
            while (getline(file, line)) {
                source.append(line + "\n");
            }
            return String(source);
        }
    };
}

#endif /* FILE_H */
