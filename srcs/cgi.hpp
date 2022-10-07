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
#include <sys/types.h>
#include <sys/wait.h>

class Cgi {

    public:

    Cgi(){}
    ~Cgi(){
        if (_envp)
            free(_envp);
        if (_argz[0])
            free(_argz[0]);
        if (_argz[1])
            free(_argz[1]);
        if (_argz)
            free(_argz);
    }
    Cgi(int request, std::vector<std::string> env)
    {
        _request = request;
        _env = env;
        execving();
    }
    void execving();
    void finding_path();
    void converting_argz();
    void converting_env();
    void forking();
    void print_inputs();
    int getResult() { return (_fd[0]); }

    private:

    int                         _request;
    std::vector<std::string>    _env;
    std::string                 _path;
    char **                     _argz;
    char **                     _envp;
    int                         _pid;
	int	                        _fd[2];

    class PipeException : public std::exception {virtual const char* what() const throw(){return ("An error occured while creating the pipe during the cgi processing");}};
    class ForkException : public std::exception {virtual const char* what() const throw(){return ("An error occured while creating the fork during the cgi processing");}};
};
