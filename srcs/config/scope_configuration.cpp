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
    std::vector<std::string> _chunks;
    while (((_file.find("http", 0) >= 0) && (_file.find_first_not_of(" \t\v\n\r\f", _file.find("http", 0) + 4) < _file.size() && _file[_file.find_first_not_of(" \t\v\n\r\f", _file.find("http", 0) + 4)] == '{')) || \
        ((_file.find("server", 0) >= 0) && (_file.find_first_not_of(" \t\v\n\r\f", _file.find("http", 0) + 4) < _file.size() && _file[_file.find_first_not_of(" \t\v\n\r\f", _file.find("server", 0) + 6)] == '{')))
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
            while ((_file.find("server") > 0 && (_file[_file.find_first_not_of(" \t\v\n\r\f", 6)] != '{')))
                _file.erase(0, 1);
            std::string chunk = chunking();
            _chunks.push_back(chunk);
            for (unsigned int i = 0; i < chunk.size(); ++i)
                _file.erase(0, 1);
        }
    }
    for (std::vector<std::string>::iterator it = _chunks.begin(); it != _chunks.end(); ++it)
        _http_blocks.push_back(HttpScope(*it));
}
void Configuration::print_resultings() {
    std::ofstream ofs;
    ofs.open("./configurations/parsed.txt");
    std::ofstream ifs;
    ifs.open("./configurations/location_parsed.txt");
    for (std::vector<ServerScope>::iterator it_two = _server_blocks.begin(); it_two != _server_blocks.end(); ++it_two)
    {
        ofs << "\n            -------| THE SERVER SCOPE |-------            \n" << std::endl;

        ofs << "    address: " << it_two->getAddress() << std::endl;
        ofs << "    port: " << it_two->getPort() << std::endl;
        std::map<std::string, std::string> getListen = it_two->getListen();
        for (std::map<std::string, std::string>::iterator it = getListen.begin(); it != getListen.end(); ++it)
            ofs << "    listen: " << "the address is " << it->first << " and the port is " << it->second << std::endl;
        std::vector<std::string> getServerName = it_two->getServerName();
        for (std::vector<std::string>::iterator it_three = getServerName.begin(); it_three != getServerName.end(); ++it_three)
            ofs << "    server_name: " << *it_three << std::endl;
        std::vector<std::string> getAllowMethod = it_two->getAllowMethod();
        for (std::vector<std::string>::iterator it_three = getAllowMethod.begin(); it_three != getAllowMethod.end(); ++it_three)
            ofs << "    allow_method: " << *it_three << std::endl;
        if (it_two->getClientBodyBufferMax().size() != 0)
            ofs << "    client_body_buffer_size: " << it_two->getClientBodyBufferMax() << std::endl;
        std::map<std::string, std::string> getDefaultErrorPage = it_two->getDefaultErrorPage(); //to avoid dangling pointer warning
        for (std::map<std::string, std::string>::iterator it_three = getDefaultErrorPage.begin(); it_three != getDefaultErrorPage.end(); ++it_three)
            ofs << "    default error page " << it_three->first << " associated to path " << it_three->second << std::endl;
        std::vector<std::string> getIndex = it_two->getIndex();
        for (std::vector<std::string>::iterator it_three = getIndex.begin(); it_three != getIndex.end(); ++it_three)
            ofs << "    index: " << *it_three << std::endl;
        ofs << "    autoindex: " << it_two->getAutoIndex() << std::endl;
        ofs << "    root: " << it_two->getRoot() << std::endl;
        std::map<std::string, std::string> getCgi = it_two->getCgi();
        for (std::map<std::string, std::string>::iterator it_three = getCgi.begin(); it_three != getCgi.end(); ++it_three)
            ofs << "    cgi: extension " << it_three->first << " associated to path " << it_three->second << std::endl;
        if (it_two->getLocations().size() > 0)
            location_recursivity(it_two->getLocations(), 1);
    }
}
void Configuration::location_recursivity(std::vector<LocationScope> location_block, int level){
    std::ofstream ofs;
    ofs.open("./configurations/location_parsed.txt", std::ios_base::app);
    for (std::vector<LocationScope>::iterator it_three = location_block.begin(); it_three != location_block.end(); ++it_three)
    {
        if (level == 1)
            ofs << "\n               ---| THE LOCATION SCOPE |---            \n" << std::endl;
        else
            ofs << "\n                    recursion level " << level << "\n" << std::endl;
        ofs << "        main_path: " << it_three->getMainPath() << std::endl;
        std::vector<std::string> getAllowMethod = it_three->getAllowMethod();
        for (std::vector<std::string>::iterator it_five = getAllowMethod.begin(); it_five != getAllowMethod.end(); ++it_five)
            ofs << "        allow_method: " << *it_five << std::endl;
        ofs << "        client_body_buffer_size: " << it_three->getClientBodyBufferMax() << std::endl;
        std::map<std::string, std::string> getDefaultErrorPage = it_three->getDefaultErrorPage(); //to avoid dangling pointer warning
        for (std::map<std::string, std::string>::iterator it_five = getDefaultErrorPage.begin(); it_five != getDefaultErrorPage.end(); ++it_five)
            ofs << "        default error page " << it_five->first << " associated to path " << it_five->second << std::endl;
        std::vector<std::string> getIndex = it_three->getIndex();
        for (std::vector<std::string>::iterator it_five = getIndex.begin(); it_five != getIndex.end(); ++it_five)
            ofs << "        index: " << *it_five << std::endl;
        ofs << "        autoindex: " << it_three->getAutoIndex() << std::endl;
        ofs << "        root: " << it_three->getRoot() << std::endl;
        ofs << "        root_in: " << it_three->getRootIn() << std::endl;
        std::map<std::string, std::string> getCgi = it_three->getCgi();
        for (std::map<std::string, std::string>::iterator it_five = getCgi.begin(); it_five != getCgi.end(); ++it_five)
            ofs << "        cgi: extension " << it_five->first << " associated to path " << it_five->second << std::endl;
        if (it_three->getLocations().size() > 0)
            location_recursivity(it_three->getLocations(), level + 1);
    }
}
void Configuration::transfer_servers(){

    for (std::vector<HttpScope>::iterator it_one = _http_blocks.begin(); it_one != _http_blocks.end(); ++it_one)
    {
        std::vector<ServerScope> getServers = it_one->getServers();
        for (std::vector<ServerScope>::iterator it_two = getServers.begin(); it_two != getServers.end(); ++it_two)
        {
            _server_blocks.push_back(*it_two);
        }
    }
}
