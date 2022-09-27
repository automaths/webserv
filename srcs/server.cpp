#include "server.hpp"

int main()
{
    class Server test;

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