#include "cgi.hpp"

void Cgi::execving() {
    finding_path();
    converting_argz();
    converting_env();
    if (pipe(_fd) == -1)
        throw PipeException();
    _pid = fork();
    if (_pid == -1)
        throw ForkException();
    if (_pid == 0)
        forking();
    else
        close(_fd[1]);
}

void Cgi::finding_path(){
    for (std::vector<std::string>::iterator it = _env.begin(); it != _env.end(); ++it)
        if (it->find("PATH_INFO=") != std::string::npos)
            _path = it->substr(it->find_first_of('=') + 1, it->size() - it->find_first_of('='));
}

void Cgi::converting_argz(){
    _argz = (char **)malloc(sizeof(char *) * 3);
    _argz[0] = strdup("coucou");
    for (std::vector<std::string>::iterator it = _env.begin(); it != _env.end(); ++it)
    {
        if (it->find("SCRIPT_FILENAME=") == 0)
            _argz[1] = strdup(it->substr(it->find_first_of('=') + 1, it->size() - it->find_first_of('=')).c_str());
    }
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
    if (dup2(_request, STDIN_FILENO) == -1)
        exit(1);
    if (dup2(_fd[1], STDOUT_FILENO) == -1)
        exit(1);
    close (_request);
    close(_fd[1]);
    if (execve(_path.c_str(), _argz, _envp) == -1)
        exit(1);
}

void Cgi::print_inputs() {
    int i = 0;
    std::cout << "the path is: " << _path << std::endl;
    while (_argz[i] != NULL)
        printf("the argz is: %s\n", _argz[i++]);
    i = 0;
    while (_envp[i] != NULL)
        printf("the envp is: %s\n", _envp[i++]);
}
