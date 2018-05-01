#pragma once

#include <string>
#include <sstream>
#include <vector>

using namespace std;

namespace Flux {
    class String {
    public:
        String() { }
        String(string s) : s(s) { }

        vector<String> split(char delimiter) {
            vector<String> tokens;
            stringstream ss;
            ss.str(s);
            string item;
            while (getline(ss, item, delimiter)) {
                tokens.push_back(String(item));
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

        bool operator==(const char* str) const {
            return s == str;
        }

        String operator+(const String& str) const {
            return String(s + str.str());
        }
    private:
        string s;
    };
}
