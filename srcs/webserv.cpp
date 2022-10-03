#include <csignal>
#include "server.hpp"
#include <iostream>
#include "library.hpp"
#include <fstream>
#include <sstream>
#include "config/scope_configuration.hpp"
#include "config/scope_http.hpp"
#include "config/scope_server.hpp"
#include "config/scope_location.hpp"

volatile std::sig_atomic_t	g_code = 0;

void	interrupt(int signal)
{
	g_code = signal;
}

int main(int ac, char **av)
{
	Server					webserv;
	std::ifstream			ifs;
	std::stringstream		buffer;
	std::list<ServerScope>	virtual_servers;

	std::signal(SIGINT, interrupt);
	std::signal(SIGQUIT, interrupt);
	if (ac == 2)
	{
		ifs.open(av[1]);
		if (ifs.fail())
		{
			std::cerr << "Error while opening configuration file" << std::endl;
			return (1);
		}
		ifs >> buffer.rdbuf();
		if (ifs.fail())
		{
			std::cerr << "Error while opening configuration file" << std::endl;
			return (1);
		}
		if (!ifs.eof())
		{
			std::cerr << "Configuration file too big" << std::endl;
			return (1);
		}
		std::cout << buffer.str();
		Configuration			current_config(buffer.str());
		current_config.apply_inheritance();
		virtual_servers = current_config.getHttpScope().front().getServers();
		for (std::list<ServerScope>::iterator st = virtual_servers.begin(); st != virtual_servers.end(); st++)
		{
			std::cout << "Ports:" << st->getPort() << std::endl;
			std::list<std::string>	tmp = st->getServerName();
			for (std::list<std::string>::iterator first = tmp.begin(); first != tmp.end(); first++)
			{
				std::cout << "Server_Name:" << *first << std::endl;
			}
			std::cout << "Root:" << st->getRoot()<<std::endl;
		}	
	}
    try
    {
        webserv.initing();
        webserv.execute();
    }
    catch (std::exception &e)
    {
        std::cout << "An exception has been caught: " << e.what() << std::endl;
    }
}
