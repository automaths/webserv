#pragma once

#include "../library.hpp"
#include <sys/types.h>
#include <dirent.h>

class ListDirectory {

    public:

    ListDirectory(){}
    ~ListDirectory(){}

    ListDirectory(std::string directory)
    {
        DIR *d;
        struct dirent *dir;
        d = opendir(directory.c_str());
        if (d)
        {
            while ((dir = readdir(d)) != NULL)
                printf("%s\n", dir->d_name);
            closedir(d);
        }
        else
            std::cout << "problems" << std::endl;
    }

    private:

    std::string    _directory;
    std::string    _root;

};