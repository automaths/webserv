#include "library.hpp"
#include "scope_configuration.hpp"

int main()
{
	std::ifstream           ifs;
	std::stringstream		buffer;

    ifs.open("./configurations/configuration.txt");
    if (ifs.fail())
        return (std::cerr << "Open Error" << std::endl, 0);
    ifs >> buffer.rdbuf();
    if (!ifs.eof())
        return (std::cerr << "File Size Error" << std::endl, 0);
    std::string configuration_file = buffer.str();
    try 
    {
        Configuration la_config(configuration_file);
        std::ofstream ofs;
        ofs.open("./configurations/servers.txt");
        std::vector<ServerScope> getServer = la_config.getServers();
        for (std::vector<ServerScope>::iterator it = getServer.begin(); it != getServer.end(); ++it)
            it->print_result();
    }
    catch (std::exception &e) 
    {
        std::cout << "An exception has been caught: " << e.what() << std::endl;
    }


    return 0;
}


