#pragma once

#include <string>
#include <fstream>

#include <exception>

namespace GDT
{
    struct FileNotFoundException : std::exception
    {
        FileNotFoundException(std::string filePath);

        const char* what() const throw();

    private:
        std::string _errorMessage;
    };

    std::string loadFile(std::string filePath);
}
