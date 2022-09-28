#include "config.hpp"

void Config::check_brackets(){
    std::string::iterator it = _file.begin();
    int n = 0;
    while (it != _file.end())
    {
        if (*it == '{')
            --n;
        if (*it == '}')
            ++n;
        ++it;
    }
    if (n != 0)
        throw BracketsException();
};