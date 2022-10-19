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

    Cgi();
    ~Cgi();
    Cgi(std::string targetFilePath, std::string exec, std::vector<std::string> env, int fd_input);
    void                            execving();
    void                            converting_argz();
    void                            converting_env();
    void                            forking();
    void                            print_inputs();
    void                            freeing();
    int                             getResult();

    private:

    std::string                     _target;
    std::string                     _exec;
    std::vector<std::string>        _env;
    int                             _fd_input;
    char **                         _argz;
    char **                         _envp;
    int                             _pid;
	int	                            _fd[2];

    class PipeException : public std::exception {virtual const char* what() const throw(){return ("An error occured while creating the pipe during the cgi processing");}};
    class ForkException : public std::exception {virtual const char* what() const throw(){return ("An error occured while creating the fork during the cgi processing");}};
    class ExecveException : public std::exception {virtual const char* what() const throw(){return ("An error occured while running execve during the cgi processing");}};
};
