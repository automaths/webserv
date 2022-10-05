#pragma once

#include "../config/library.hpp"
#include <stdlib.h>

class Cgi {

    public:

    Cgi(){}
    ~Cgi(){
        // if (_envp)
        //     free(_envp);
        // free(_argz[2]);
        // free(_argz[1]);
        // free(_argz[0]);
        // free(_argz);
    }

    Cgi(int request, std::vector<std::string> env)
    {
        _request = request;
        _env = env;
        execving();
        // _num = 0;
        // waitpid(_pid, &_num, 0);

        // char buffer[500];
        // int size = read(_fd[1], &buffer, 500);
        // (void)size;
        // printf("%s", buffer);
    }
    void execving() 
    {
        finding_path();
        std::cout << _path << std::endl;
        converting_argz();
        int i = 0;
        while (_argz[i] != NULL)
            printf("the argz is: %s\n", _argz[i++]);
        // converting_env();
        // i = 0;
        // while (_envp[i] != NULL)
        //     printf("the envp is: %s\n", _envp[i++]);
        // if (pipe(_fd) == -1)
        //     return ;
        // _pid = fork();
        // if (_pid == -1)
        //     return ;
        // if (_pid == 0)
        //     forking();
        // else
        //     close(_fd[1]);
    }
    void finding_path()
    {
        for (std::vector<std::string>::iterator it = _env.begin(); it != _env.end(); ++it)
            if (it->find("PATH_INFO=") != std::string::npos)
                _path = it->substr(it->find_first_of('=') + 1, it->size() - it->find_first_of('='));
    }
    void converting_argz()
    {
        _argz = (char **)malloc(sizeof(char *) * 3);
        _argz[0] = strdup("coucou");
        char buffer[1048576];
        int size = read(_request, &buffer, 1048576);
        if (size == -1)
            return ;
        _argz[1] = (char*)malloc(sizeof(char) * size);
        _argz[1][size] = '\0';
        int n = -1;
        while (++n < size)
            _argz[1][n] = buffer[n];
        _argz[2] = NULL;
    }
    void converting_env()
    {
        int i = 0;
        _envp = (char **)malloc(sizeof(char *) * (_env.size() + 1));
        _envp[_env.size()] = NULL;
        for (std::vector<std::string>::iterator it = _env.begin(); it != _env.end(); ++it)
            _envp[i++] = (char *)it->c_str(); 
    }
    void forking() 
    {
        if (dup2(_request, STDIN_FILENO) == -1)
            return ;
        if (dup2(_fd[1], STDOUT_FILENO) == -1)
            return ;
        close (_request);
        close(_fd[1]);
        if (execve(_path.c_str(), _argz, _envp) == -1)
            return ;
    }

    private:

    int _request;
    std::vector<std::string> _env;

    // int _pid;
	int	_fd[2];

    char **_argz;
    char **_envp;
    std::string _path;

    std::pair<std::string, std::string> _result;
    // int                                 _num;
};

// void	waitpiding(t_command *cmd)
// {
// 	t_command	*tmp;
// 	int			num;
// 	tmp = cmd;
// 	num = 0;
// 	while (tmp != NULL)
// 	{
// 		if (tmp->is_exec == 1 && tmp->arg->pid)
// 		{
// 			if (waitpid(tmp->arg->pid, &num, 0) != -1)
// 			{
// 				if (tmp->arg->pid)
// 					updating_singleton(num);
// 			}
// 		}
// 		tmp = tmp->next;
// 	}
// }
// void	forking(t_command *cmd)
// {
// 	if (cmd->is_piping && cmd->fd[0] != 0 && cmd->fd[0] != -1)
// 		close(cmd->fd[0]);
// 	if (cmd->fd_in != 0 && cmd->fd_in != -1)
// 	{
// 		if (dup2(cmd->fd_in, STDIN_FILENO) == -1)
// 			return ;
// 	}
// 	if (cmd->fd_out != 1 && cmd->fd_out != -1)
// 	{
// 		if (dup2(cmd->fd_out, STDOUT_FILENO) == -1)
// 			return ;
// 	}
// 	closing_next_fds(cmd);
// 	if (cmd->previous_fd != 0 && cmd->previous_fd != 1 \
// 		&& cmd->previous_fd != -1)
// 		close(cmd->previous_fd);
// 	if (cmd->fd_in != 0 && cmd->fd_in != -1)
// 		close(cmd->fd_in);
// 	if (cmd->fd_out != 1 && cmd->fd_out != -1)
// 		close(cmd->fd_out);
// 	if (execve(cmd->arg->path, cmd->arg->argz, cmd->envp_char) == -1)
// 		return ;
// }
// void	exec_token(t_command *cmd)
// {
// 	cmd->envp_char = envp_to_char(cmd->env);
// 	cmd->arg->pid = fork();
// 	if (cmd->arg->pid == -1)
// 		return ;
// 	if (cmd->arg->pid == 0)
// 	{
// 		signal(SIGINT, exit_fork);
// 		forking(cmd);
// 	}
// 	else
// 	{
// 		if (cmd->fd_in != 0 && cmd->fd_in != -1)
// 			close(cmd->fd_in);
// 		if (cmd->fd_out != 1 && cmd->fd_out != -1)
// 			close(cmd->fd_out);
// 		if (cmd->previous_fd != 0 && cmd->previous_fd != 1 \
// 			&& cmd->previous_fd != -1)
// 			close(cmd->previous_fd);
// 	}
// }