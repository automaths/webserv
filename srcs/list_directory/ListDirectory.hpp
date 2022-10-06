#pragma once

#include "../library.hpp"
#include <sys/types.h>
#include <dirent.h>

class ListDirectory {

    public:

    ListDirectory(){}
    ~ListDirectory(){}
    ListDirectory(std::string directory, std::string uri)
    {
        DIR *d;
        struct dirent *dir;
        std::string bullet;
        d = opendir(directory.c_str());
        if (d)
        {
            while ((dir = readdir(d)) != NULL)
            {
                if (strcmp(dir->d_name, "..") && strcmp(dir->d_name, "."))
                {
                    bullet.clear();
                    bullet = "<li><a href=\'" + uri + '/' + dir->d_name + "\'>" + dir->d_name + "</a></li>\n";
                    _listing.push_back(bullet);
                }
            }
            closedir(d);
        }
        else
            throw OpenDirException();
    }
    std::string listing() {
        std::string body = "<!DOCTYPE html>\n<html lang='en'>\n<head>\n<meta charset='UTF-8'>\n<meta http-equiv='X-UA-Compatible' content='IE=edge'>\n<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n<title>Index $uri</title>\n</head>\n<body>\n<h1>$uri</h1>\n<ul>\n";
        for (std::vector<std::string>::iterator it = _listing.begin(); it != _listing.end(); ++it)
            body += *it;
        body += "</ul>\n</body>\n</html>";
        std::cout << body << std::endl;
        return (body);
    }

    private:

    std::vector<std::string> _listing;
    std::string    _directory;
    std::string    _root;
    class OpenDirException : public std::exception {virtual const char* what() const throw(){return ("An error occured while opening the directory in the listing directory process");}};
};