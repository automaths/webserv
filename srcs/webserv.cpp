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
	static_cast<void>(signal);
	g_code = 2;
}

int main(int ac, char **av)
{
	Server					webserv;
	std::ifstream			ifs;
	std::stringstream		buffer;
	std::vector<ServerScope>	virtual_servers;

	std::signal(SIGINT, interrupt);
	std::signal(SIGQUIT, interrupt);
	if (ac > 2)
	{
		std::cerr << "Too many configuration files supplied, this program expects at most one argument" << std::endl;
		return (1);
	}
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
		ifs.close();
	}
	else
		buffer << "http {" << std::endl << "server {" << std::endl << "listen 8080;" << std::endl << "}" << std::endl << "}" << std::endl;
	try
    {
		Configuration			current_config(buffer.str());
		virtual_servers = current_config.getServers();

        webserv.initing(virtual_servers);
        webserv.execute();
    }
    catch (std::exception const & e)
    {
        std::cerr << "An exception has been caught: " << e.what() << std::endl;
    }
	return (0);
}
