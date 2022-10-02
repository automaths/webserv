#include "library.hpp"
#include "scope_configuration.hpp"

int main()
{
	std::ifstream ifs;
    ifs.open("./configurations/configuration.txt");
    std::ofstream ofs;
    ofs.open("./configurations/parsed.txt");
    std::string buffer; 
    char c = ifs.get();
    while (ifs.good())
    {
        buffer.append(1, c);
        c = ifs.get();
    }
    try 
    {
        Configuration la_config(buffer);
    }
    catch (std::exception &e) 
    {
        std::cout << "An exception has been caught: " << e.what() << std::endl;
    }
    return 0;
}


