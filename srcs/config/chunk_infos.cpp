#include "library.hpp"
#include "chunk_infos.hpp"

void Chunk_Infos::extract_listen(std::string listen_dir){
    listen_dir.erase(0, listen_dir.find("listen") + 6);
    if (listen_dir.find_first_of(':', 0) != std::string::npos)
    {
        _address = listen_dir.substr(0, listen_dir.find_first_of(':', 0));
        listen_dir.erase(0, listen_dir.find_first_of(':', 0) + 1);
        _port = listen_dir;
    }
    else
    {
        if (listen_dir.find_first_not_of("\t\v\n\r\f 0123456789") == std::string::npos)
        {
            _port = listen_dir;
            _address = "*";
        }
        else
        {
            _port = "80";
            _address = listen_dir;
        }
    }
}

void Chunk_Infos::extract_server_name(std::string server_name_dir) {
    server_name_dir.erase(0, server_name_dir.find("server_name") + 12);
    while (server_name_dir.find_first_of(" \t\v\n\r\f", 0) == 0)
        server_name_dir.erase(0, 1);
    while (server_name_dir.size() != 0)
    {  
        if (server_name_dir.find_first_of(" \t\v\n\r\f", 0) != std::string::npos)
        {
            _server_names.push_back(server_name_dir.substr(0, server_name_dir.find_first_of(" \t\v\n\r\f", 0)));
            server_name_dir.erase(0, server_name_dir.find_first_of(" \t\v\n\r\f", 0));
        }
        else
        {
            _server_names.push_back(server_name_dir.substr(0, server_name_dir.size()));
            server_name_dir.erase(0, server_name_dir.size());
        }
        while(server_name_dir.find_first_of(" \t\v\n\r\f", 0) == 0)
            server_name_dir.erase(0, 1);
    }
}

void Chunk_Infos::extract_client_body_buffer_size(std::string body_max) {
    body_max.erase(0, body_max.find("client_body_buffer_size") + 23);
    _client_body_buffer_size = body_max.substr(body_max.find_first_not_of("\t\v\n\r\f "), body_max.find_first_of("\t\v\n\r\f ", body_max.find_first_not_of("\t\v\n\r\f ")));
}

void Chunk_Infos::extract_root(std::string root_dir) {
    root_dir.erase(0, root_dir.find_first_of("root") + 4);
    _root = root_dir.substr(root_dir.find_first_not_of("\t\v\n\r\f ", 0), root_dir.find_first_of("\t\v\n\r\f ", root_dir.find_first_not_of("\t\v\n\r\f ", 0)));
}

void Chunk_Infos::extract_allow_method(std::string allow_method_dir) {
    allow_method_dir.erase(0, allow_method_dir.find("allow_method") + 12);
    while (allow_method_dir.find_first_of(" \t\v\n\r\f", 0) == 0)
        allow_method_dir.erase(0, 1);
    while (allow_method_dir.size() != 0)
    {  
        if (allow_method_dir.find_first_of(" \t\v\n\r\f", 0) != std::string::npos)
        {
            _allow_method.push_back(allow_method_dir.substr(0, allow_method_dir.find_first_of(" \t\v\n\r\f", 0)));
            allow_method_dir.erase(0, allow_method_dir.find_first_of(" \t\v\n\r\f", 0));
        }
        else
        {
            _allow_method.push_back(allow_method_dir.substr(0, allow_method_dir.size()));
            allow_method_dir.erase(0, allow_method_dir.size());
        }
        while(allow_method_dir.find_first_of(" \t\v\n\r\f", 0) == 0)
            allow_method_dir.erase(0, 1);
    }
}

void Chunk_Infos::extract_cgi(std::string cgi_dir) {
    std::list<std::string> content;
    cgi_dir.erase(0, cgi_dir.find_first_of("cgi") + 3);
    while (cgi_dir.find_first_of(" \t\v\n\r\f", 0) == 0)
        cgi_dir.erase(0, 1);
    while (cgi_dir.size() != 0)
    {  
        if (cgi_dir.find_first_of(" \t\v\n\r\f", 0) != std::string::npos)
        {
            content.push_back(cgi_dir.substr(0, cgi_dir.find_first_of(" \t\v\n\r\f", 0)));
            cgi_dir.erase(0, cgi_dir.find_first_of(" \t\v\n\r\f", 0));
        }
        else
        {
            content.push_back(cgi_dir.substr(0, cgi_dir.size()));
            cgi_dir.erase(0, cgi_dir.size());
        }
        while(cgi_dir.find_first_of(" \t\v\n\r\f", 0) == 0)
            cgi_dir.erase(0, 1);
    }
    for (std::list<std::string>::iterator it = content.begin(); it != content.end(); ++it)
    {
        if (++it != content.end())
        {
            _cgi.push_back(std::make_pair(*(--it), *(++it)));
        }
        else
            --it;
    }
}

void Chunk_Infos::extract_index(std::string index_dir) {
    index_dir.erase(0, index_dir.find("index") + 5);
    while (index_dir.find_first_of(" \t\v\n\r\f", 0) == 0)
        index_dir.erase(0, 1);
    while (index_dir.size() != 0)
    {  
        if (index_dir.find_first_of(" \t\v\n\r\f", 0) != std::string::npos)
        {
            _index.push_back(index_dir.substr(0, index_dir.find_first_of(" \t\v\n\r\f", 0)));
            index_dir.erase(0, index_dir.find_first_of(" \t\v\n\r\f", 0));
        }
        else
        {
            _index.push_back(index_dir.substr(0, index_dir.size()));
            index_dir.erase(0, index_dir.size());
        }
        while(index_dir.find_first_of(" \t\v\n\r\f", 0) == 0)
            index_dir.erase(0, 1);
    }
}

void Chunk_Infos::extract_default_error_pages(std::string error_page_dir) {
    error_page_dir.erase(0, error_page_dir.find("error_page") + 10);
    while (error_page_dir.find_first_of(" \t\v\n\r\f", 0) == 0)
        error_page_dir.erase(0, 1);
    std::list<std::string> number;
    std::string path;
    while (error_page_dir.size() != 0)
    {  
        if (error_page_dir.find_first_of(" \t\v\n\r\f", 0) != std::string::npos)
        {
            number.push_back(error_page_dir.substr(0, error_page_dir.find_first_of(" \t\v\n\r\f", 0)));
            error_page_dir.erase(0, error_page_dir.find_first_of(" \t\v\n\r\f", 0));
        }
        else
        {
            number.push_back(error_page_dir.substr(0, error_page_dir.size()));
            error_page_dir.erase(0, error_page_dir.size());
        }
        while(error_page_dir.find_first_of(" \t\v\n\r\f", 0) == 0)
            error_page_dir.erase(0, 1);
    }
    path = number.back();
    number.pop_back();
    for (std::list<std::string>::iterator it = number.begin(); it != number.end(); ++it)
        _default_error_pages.insert(std::make_pair(*it, path));
}

void Chunk_Infos::extract_autoindex(std::string autoindex_dir) {
    autoindex_dir.erase(0, autoindex_dir.find("autoindex ") + 9);
    _autoindex = autoindex_dir.substr(autoindex_dir.find_first_not_of("\t\v\n\r\f ", 0), autoindex_dir.find_first_of("\t\v\n\r\f ", autoindex_dir.find_first_not_of("\t\v\n\r\f ", 0)));
    if (_autoindex.compare("on") != 0)
        _autoindex = "off";
}

void Chunk_Infos::extract_try_files(std::string try_files_dir) {
    try_files_dir.erase(0, try_files_dir.find("try_files") + 9);
    while (try_files_dir.find_first_of(" \t\v\n\r\f", 0) == 0)
        try_files_dir.erase(0, 1);
    while (try_files_dir.size() != 0)
    {  
        if (try_files_dir.find_first_of(" \t\v\n\r\f", 0) != std::string::npos)
        {
            _try_files.push_back(try_files_dir.substr(0, try_files_dir.find_first_of(" \t\v\n\r\f", 0)));
            try_files_dir.erase(0, try_files_dir.find_first_of(" \t\v\n\r\f", 0));
        }
        else
        {
            _try_files.push_back(try_files_dir.substr(0, try_files_dir.size()));
            try_files_dir.erase(0, try_files_dir.size());
        }
        while(try_files_dir.find_first_of(" \t\v\n\r\f", 0) == 0)
            try_files_dir.erase(0, 1);
    }
}

void Chunk_Infos::extract_location_blocks() {  
    std::string copy = _chunk;
    while (copy.find(';', 0) != std::string::npos || copy.find('}') != std::string::npos)
    {
        if (copy.find('}', 0) == std::string::npos)
            copy.erase(0, copy.find(';', 0) + 1);
        else if (copy.find(';', 0) == std::string::npos)
            copy.erase(0, copy.find('}', 0) + 1);
        else 
        {
            if (copy.find(';', 0) < copy.find('}', 0))
                copy.erase(0, copy.find(';', 0) + 1);
            else
                copy.erase(0, copy.find('}', 0) + 1);
        }
        if (copy.find_first_not_of("\t\v\n\r\f ", 0) == copy.find("location", 0) && copy.find("location", 0) != std::string::npos)
        {
            std::string::iterator it = copy.begin();
            for (unsigned int i = 0; i < copy.find("location", 0) + 1; ++i)
                ++it;
            std::string::iterator tmp = it;
            while (it != copy.end() && *it != '{')
                ++it;
            ++it;
            int n = 1;
            while (it != copy.end() && n > 0)
            {
                if (*it == '{')
                    ++n;
                if (*it == '}')
                    --n;
                ++it;
            }
            n = 0;
            while (tmp != it)
            {
                ++n;
                ++tmp;
            }
            _location_blocks.push_back(copy.substr(copy.find("location", 0), n + 1));
            _chunk.erase(_chunk.find(_location_blocks.back(), 0), _location_blocks.back().size());
        }
    }
}

void Chunk_Infos::extract_directives() {
    while (_chunk.find(';', 0) != std::string::npos)
    {
        _directives.push_back(_chunk.substr(0, _chunk.find(';')));
        _chunk.erase(0, _chunk.find(';', 0) + 1);
    }
}

void Chunk_Infos::print_result() {
    std::cout << "the address is: " << _address << std::endl;
    std::cout << "the port is: " << _port << std::endl;
    for (std::list<std::string>::iterator it = _server_names.begin(); it != _server_names.end(); ++it)
        std::cout << "server_name: " << *it << std::endl;
    for (std::list<std::string>::iterator it = _index.begin(); it != _index.end(); ++it)
        std::cout << "index: " << *it << std::endl;
    for (std::map<std::string, std::string>::iterator it = _default_error_pages.begin(); it != _default_error_pages.end(); ++it)
        std::cout << "error page " << it->first << " associated path " << it->second << std::endl;
    std::cout << "body max: " << _client_body_buffer_size << std::endl;
    std::cout << "root: " << _root << std::endl;
    std::cout << "allowed method: ";
    for (std::list<std::string>::iterator it = _allow_method.begin(); it != _allow_method.end(); ++it)
        std::cout << *it << ", ";
    std::cout << std::endl;
    for (std::list<std::pair<std::string, std::string> >::iterator it = _cgi.begin(); it != _cgi.end(); ++it)
        std::cout << "cgi: " << it->first << " associated to path " << it->second << std::endl;
    std::cout << "autoindex: " << _autoindex << std::endl;
    for (std::list<std::string>::iterator it = _try_files.begin(); it != _try_files.end(); ++it)
    {
        if (++it != _try_files.end())
            std::cout << "try_files: " << *(--it) << std::endl;
        else
            std::cout << "try_files (fallback): " << *(--it) << std::endl;
    }
    for (std::list<std::string>::iterator it = _location_blocks.begin(); it != _location_blocks.end(); ++it)
        std::cout << "location block: " << *it << std::endl;

    // std::ofstream ofs;
    // ofs.open("config_result.txt");
    // ofs << _chunk;
    // ofs << "THE CONFIGS\n" << std::endl;
    // for (std::list<std::string>::iterator it = _configs.begin(); it != _configs.end(); ++it)
    //     ofs << *it;
    // ofs << "\n\nTHE LOCATIONS\n" << std::endl;
}
