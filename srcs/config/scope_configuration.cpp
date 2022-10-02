#include "scope_configuration.hpp"

void Configuration::check_brackets(){
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

std::string Configuration::chunking() {
    std::string::iterator it = _file.begin();
    while (*it != '{')
        ++it;
    ++it;
    int n = 1;
    while (it != _file.end() && n > 0)
    {
        if (*it == '{')
            ++n;
        if (*it == '}')
            --n;
        ++it;
    }
    return (_file.substr(0, it - _file.begin()));
}

void Configuration::extract_http_blocks() {
    std::list<std::string> _chunks;
    while (((_file.find("http", 0) >= 0) && (_file[_file.find_first_not_of(" \t\v\n\r\f", _file.find("http", 0) + 4)] == '{')) || \
        ((_file.find("server", 0) >= 0) && (_file[_file.find_first_not_of(" \t\v\n\r\f", _file.find("server", 0) + 6)] == '{')))
    {
        if (_file.find("http", 0) < _file.find("server") && _file.find("http", 0) != std::string::npos)
        {
            while ((_file.find("http") > 0 && (_file[_file.find_first_not_of(" \t\v\n\r\f", 4)] != '{')))
                _file.erase(0, 1);
            std::string chunk = chunking();
            _chunks.push_back(chunk);
            for (unsigned int i = 0; i < chunk.size(); ++i)
                _file.erase(0, 1);
        }
        else
        {
            while ((_file.find("server") > 0 && (_file[_file.find_first_not_of(" \t\v\n\r\f", 4)] != '{')))
                _file.erase(0, 1);
            std::string chunk = chunking();
            _chunks.push_back(chunk);
            for (unsigned int i = 0; i < chunk.size(); ++i)
                _file.erase(0, 1);
        }
    }
    for (std::list<std::string>::iterator it = _chunks.begin(); it != _chunks.end(); ++it)
        _http_blocks.push_back(HttpScope(*it));
}
