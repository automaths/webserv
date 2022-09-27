#include "library.hpp"

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

    Config el_configo(buffer);
}