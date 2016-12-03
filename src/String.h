#pragma once
#ifndef STRING_H
#define STRING_H

#include <string>
#include <sstream>
#include <vector>

using namespace std;

namespace Flux {
    class String {
    public:
        String(string s) : s(s) { }

        vector<string> split(char delimiter) {
            vector<string> tokens;
            stringstream ss;
            ss.str(s);
            string item;
            while (getline(ss, item, delimiter)) {
                tokens.push_back(item);
            }
            return tokens;
        }

        string str() const {
            return s;
        }

        const char* c_str() const {
            return s.c_str();
        }

        /* Operator overloads */
        bool operator==(const String& str) const {
            return s == str.str();
        }

        String operator+(const String& str) const {
            return String(s + str.str());
        }
    private:
        string s;
    };
}

#endif /* STRING_H */
