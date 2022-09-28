#include "library.hpp"
#include "config.hpp"

int main()
{
	std::ifstream ifs;
    std::string buffer; 

    ifs.open("config.txt");
    char c = ifs.get();
    while (ifs.good()){
        buffer.append(1, c);
        c = ifs.get();
    }

    try {
        Config el_configo(buffer);
    }
    catch (std::exception &e) {
        std::cout << "An exception has been caught: " << e.what() << std::endl;
    }
    return 0;
}


