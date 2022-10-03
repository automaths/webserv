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

    void Configuration::print_configurations() {

        std::ofstream ofs;
        ofs.open("./configurations/parsed.txt");
        std::ofstream ofss;
        ofss.open("./configurations/location_parsed.txt");

        ofs << "------------| PRINTING THE CONFIGURATION FILE |------------" << std::endl;
        
        for (std::list<HttpScope>::iterator it_one = _http_blocks.begin(); it_one != _http_blocks.end(); ++it_one)
        {
            ofs << "\n     ---------------| THE HTTP SCOPE |---------------        \n" << std::endl;

            if (it_one->getClientBodyBufferMax().size() != 0)
                ofs << "client_body_buffer_size: " << it_one->getClientBodyBufferMax() << std::endl;
            std::map<std::string, std::string> getDefaultErrorPage = it_one->getDefaultErrorPage(); //to avoid dangling pointer warning
            for (std::map<std::string, std::string>::iterator it_two = getDefaultErrorPage.begin(); it_two != getDefaultErrorPage.end(); ++it_two)
                ofs << "default error page " << it_two->first << " associated to path " << it_two->second << std::endl;
            std::list<std::string> getIndex = it_one->getIndex();
            for (std::list<std::string>::iterator it_two = getIndex.begin(); it_two != getIndex.end(); ++it_two)
                ofs << "index: " << *it_two << std::endl;
            ofs << "autoindex: " << it_one->getAutoIndex() << std::endl;
            ofs << "root: " << it_one->getRoot() << std::endl;
            std::map<std::string, std::string> getCgi = it_one->getCgi();
            for (std::map<std::string, std::string>::iterator it_two = getCgi.begin(); it_two != getCgi.end(); ++it_two)
                ofs << "cgi: extension " << it_two->first << " associated to path " << it_two->second << std::endl;
            std::list<ServerScope> getServers = it_one->getServers();
            for (std::list<ServerScope>::iterator it_two = getServers.begin(); it_two != getServers.end(); ++it_two)
            {
                ofs << "\n            -------| THE SERVER SCOPE |-------            \n" << std::endl;

                ofs << "    address: " << it_two->getAddress() << std::endl;
                ofs << "    port: " << it_two->getPort() << std::endl;
                std::list<std::string> getServerName = it_two->getServerName();
                for (std::list<std::string>::iterator it_three = getServerName.begin(); it_three != getServerName.end(); ++it_three)
                    ofs << "    server_name: " << *it_three << std::endl;
                std::list<std::string> getTryFiles = it_two->getTryFiles();
                for (std::list<std::string>::iterator it_three = getTryFiles.begin(); it_three != getTryFiles.end(); ++it_three)
                    ofs << "    try_files: " << *it_three << std::endl;
                std::list<std::string> getAllowMethod = it_two->getAllowMethod();
                for (std::list<std::string>::iterator it_three = getAllowMethod.begin(); it_three != getAllowMethod.end(); ++it_three)
                    ofs << "    allow_method: " << *it_three << std::endl;
                if (it_two->getClientBodyBufferMax().size() != 0)
                    ofs << "    client_body_buffer_size: " << it_two->getClientBodyBufferMax() << std::endl;
                std::map<std::string, std::string> getDefaultErrorPage = it_two->getDefaultErrorPage(); //to avoid dangling pointer warning
                for (std::map<std::string, std::string>::iterator it_three = getDefaultErrorPage.begin(); it_three != getDefaultErrorPage.end(); ++it_three)
                    ofs << "    default error page " << it_three->first << " associated to path " << it_three->second << std::endl;
                std::list<std::string> getIndex = it_two->getIndex();
                for (std::list<std::string>::iterator it_three = getIndex.begin(); it_three != getIndex.end(); ++it_three)
                    ofs << "    index: " << *it_three << std::endl;
                ofs << "    autoindex: " << it_two->getAutoIndex() << std::endl;
                ofs << "    root: " << it_two->getRoot() << std::endl;
                std::map<std::string, std::string> getCgi = it_two->getCgi();
                for (std::map<std::string, std::string>::iterator it_three = getCgi.begin(); it_three != getCgi.end(); ++it_three)
                    ofs << "    cgi: extension " << it_three->first << " associated to path " << it_three->second << std::endl;
                if (it_two->getLocations().size() > 0)
                    location_recurs(it_two->getLocations(), 1);
            }
        }
    }

    void Configuration::location_recurs(std::list<LocationScope> location_block, int level){
        std::ofstream ofs;
        ofs.open("./configurations/location_parsed.txt", std::ios_base::app);
        for (std::list<LocationScope>::iterator it_three = location_block.begin(); it_three != location_block.end(); ++it_three)
        {
            if (level == 1)
                ofs << "\n               ---| THE LOCATION SCOPE |---            \n" << std::endl;
            else
                ofs << "\n                    recursion level " << level << "\n" << std::endl;
            ofs << "        main_path: " << it_three->getMainPath() << std::endl;
            std::list<std::string> getTryFiles = it_three->getTryFiles();
            for (std::list<std::string>::iterator it_five = getTryFiles.begin(); it_five != getTryFiles.end(); ++it_five)
                ofs << "        try_files: " << *it_five << std::endl;
            std::list<std::string> getAllowMethod = it_three->getAllowMethod();
            for (std::list<std::string>::iterator it_five = getAllowMethod.begin(); it_five != getAllowMethod.end(); ++it_five)
                ofs << "        allow_method: " << *it_five << std::endl;
            ofs << "        client_body_buffer_size: " << it_three->getClientBodyBufferMax() << std::endl;
            std::map<std::string, std::string> getDefaultErrorPage = it_three->getDefaultErrorPage(); //to avoid dangling pointer warning
            for (std::map<std::string, std::string>::iterator it_five = getDefaultErrorPage.begin(); it_five != getDefaultErrorPage.end(); ++it_five)
                ofs << "        default error page " << it_five->first << " associated to path " << it_five->second << std::endl;
            std::list<std::string> getIndex = it_three->getIndex();
            for (std::list<std::string>::iterator it_five = getIndex.begin(); it_five != getIndex.end(); ++it_five)
                ofs << "        index: " << *it_five << std::endl;
            ofs << "        autoindex: " << it_three->getAutoIndex() << std::endl;
            ofs << "        root: " << it_three->getRoot() << std::endl;
            std::map<std::string, std::string> getCgi = it_three->getCgi();
            for (std::map<std::string, std::string>::iterator it_five = getCgi.begin(); it_five != getCgi.end(); ++it_five)
                ofs << "        cgi: extension " << it_five->first << " associated to path " << it_five->second << std::endl;
            if (it_three->getLocations().size() > 0)
                location_recurs(it_three->getLocations(), level + 1);
        }
    }
