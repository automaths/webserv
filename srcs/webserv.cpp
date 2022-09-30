#include <csignal>
#include "library.hpp"

volatile std::sig_atomic_t	g_code = 0;

void	interrupt(int signal)
{
	g_code = signal;
}

int main()
{
	class Server test;

	std::signal(SIGINT, interrupt);
	std::signal(SIGQUIT, interrupt);
    try
    {
        test.initing();
        test.execute();
    }
    catch (std::exception &e)
    {
        std::cout << "An exception has been caught: " << e.what() << std::endl;
    }
}
