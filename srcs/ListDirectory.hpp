#pragma once

#include <sys/types.h>
#include <dirent.h>
#include <algorithm>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
#include <exception>
#include <iostream>
#include <list>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <map>
#include <list>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

class ListDirectory {

    public:

    ListDirectory();
    ~ListDirectory();
    ListDirectory(std::string directory, std::string uri);
    std::string listing();

    private:

    std::vector<std::string> _listing;
    std::string    _directory;
    std::string    _root;
    class OpenDirException : public std::exception {virtual const char* what() const throw(){return ("An error occured while opening the directory in the listing directory process");}};
};
