#include "cgi.hpp"

void Cgi::execving() {

    converting_argz();
    converting_env();
    if (pipe(_fd) == -1)
		throw PipeException();
	std::cerr << "reading end: " << _fd[0] << " , writing end: " << _fd[1] << std::endl;
    _pid = fork();
    if (_pid == -1)
        throw ForkException();
    if (_pid == 0)
        forking();
    else
	{
        close(_fd[1]);
		if (_fd_input != -1)
			close(_fd_input);
	}
}

void Cgi::converting_argz(){
    _argz = (char **)malloc(sizeof(char *) * 3);
    _argz[0] = strdup(_exec.c_str());
    _argz[1] = (char*)_target.c_str();
    _argz[2] = NULL;
}

void Cgi::converting_env(){
    int i = 0;
    _envp = (char **)malloc(sizeof(char *) * (_env.size() + 1));
    for (std::vector<std::string>::iterator it = _env.begin(); it != _env.end(); ++it)
        _envp[i++] = (char *)it->c_str(); 
    _envp[_env.size()] = NULL;
}

void Cgi::forking() {
    if (_fd_input != -1)
    {
        if (dup2(_fd_input, STDIN_FILENO) == -1)
            exit(1);
        close (_fd_input);
    }
    if (dup2(_fd[1], STDOUT_FILENO) == -1)
        exit(1);
    close(_fd[1]);
	close(_fd[0]);
    if (execve(_argz[0], _argz, _envp) == -1)
        exit(1);
}

void Cgi::print_inputs() {
    int i = 0;
    std::cout << "the path is: " << _argz[0] << std::endl;
    while (_argz[i] != NULL)
        printf("the argz is: %s\n", _argz[i++]);
    i = 0;
    while (_envp[i] != NULL)
        printf("the envp is: %s\n", _envp[i++]);
}

//add getQuery to request