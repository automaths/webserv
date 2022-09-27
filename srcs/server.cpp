#include "library.hpp"

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
    /*
	std::ifstream ifs;
    std::string buffer; 

    ifs.open("config.txt");
    char c = ifs.get();
    while (ifs.good()){
        buffer.append(1, c);
        c = ifs.get();
    }
*/
//    Config el_configo(buffer);
}
