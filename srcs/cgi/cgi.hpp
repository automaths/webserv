#pragma once

#include "../config/library.hpp"
#include <stdlib.h>


//apply default server
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
        _num = 0;
        waitpid(_pid, &_num, 0);
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
    int                         _num;

    class PipeException : public std::exception {virtual const char* what() const throw(){return ("An error occured while creating the pipe during the cgi processing");}};
    class ForkException : public std::exception {virtual const char* what() const throw(){return ("An error occured while creating the fork during the cgi processing");}};
};
