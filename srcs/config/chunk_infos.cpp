#include "library.hpp"
#include "chunk_infos.hpp"

void Chunk_Infos::extract_address_port(){
    if (_chunk.find("listen ", 0) != std::string::npos)
    {
        std::string listen_dir = _chunk.substr(_chunk.find("listen ", 0), _chunk.find_first_of(';', _chunk.find("listen ", 0)) - _chunk.find("listen ", 0));
        listen_dir.erase(0, listen_dir.find_first_of("listen ") + 7);
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
    else
    {
        _address = "*";
        _port = "80";
    }
}

void Chunk_Infos::extract_server_name() {
    if (_chunk.find("server_name ", 0) != std::string::npos)
    {
        std::string server_name_dir = _chunk.substr(_chunk.find("server_name ", 0), _chunk.find_first_of(';', _chunk.find("server_name ", 0)) - _chunk.find("server_name ", 0));
        server_name_dir.erase(0, server_name_dir.find_first_of("server_name ") + 12);
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
    else
        _server_names.push_back("");
}

void Chunk_Infos::extract_client_body_buffer_size() {
    if (_chunk.find("client_body_buffer_size ", 0) != std::string::npos)
    {
        std::string body_max = _chunk.substr(_chunk.find("client_body_buffer_size ", 0), _chunk.find_first_of(';', _chunk.find("client_body_buffer_size ", 0)) - _chunk.find("client_body_buffer_size ", 0));
        body_max.erase(0, body_max.find_first_of("client_body_buffer_size ") + 24);
        _client_body_buffer_size = body_max.substr(body_max.find_first_not_of("\t\v\n\r\f ", 0), body_max.find_first_of("\t\v\n\r\f ", body_max.find_first_not_of("\t\v\n\r\f ", 0)));

    }
    else
        _client_body_buffer_size = "";
}

void Chunk_Infos::extract_root() {
    if (_chunk.find("root ", 0) != std::string::npos)
    {
        std::string root = _chunk.substr(_chunk.find("root ", 0), _chunk.find_first_of(';', _chunk.find("root ", 0)) - _chunk.find("root ", 0));
        root.erase(0, root.find_first_of("root ") + 5);
        _root = root.substr(root.find_first_not_of("\t\v\n\r\f ", 0), root.find_first_of("\t\v\n\r\f ", root.find_first_not_of("\t\v\n\r\f ", 0)));

    }
    else
        _root = "html";
}

void Chunk_Infos::extract_allow_method() {
    while (_chunk.find("allow_method ", 0) != std::string::npos)
    {
        std::string allow_method_dir = _chunk.substr(_chunk.find("allow_method ", 0), _chunk.find_first_of(';', _chunk.find("allow_method ", 0)) - _chunk.find("allow_method ", 0));
        //little trick here to stop while loop
        _chunk.erase(_chunk.find("allow_method ", 0), 1);
        allow_method_dir.erase(0, allow_method_dir.find_first_of("allow_method ") + 13);
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
}

void Chunk_Infos::extract_cgi() {
    std::list<std::string> content;
    while (_chunk.find("cgi ", 0) != std::string::npos)
    {
        content.clear();
        std::string cgi_dir = _chunk.substr(_chunk.find("cgi ", 0), _chunk.find_first_of(';', _chunk.find("cgi ", 0)) - _chunk.find("cgi ", 0));
        //little trick here to stop while loop
        _chunk.erase(_chunk.find("cgi ", 0), 1);
        cgi_dir.erase(0, cgi_dir.find_first_of("cgi ") + 4);
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
}

void Chunk_Infos::extract_index() {
    if (_chunk.find("index ", 0) != std::string::npos)
{
    std::string index_dir = _chunk.substr(_chunk.find("index ", 0), _chunk.find_first_of(';', _chunk.find("index ", 0)) - _chunk.find("index ", 0));
    index_dir.erase(0, index_dir.find_first_of("index ") + 6);
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
else
    _index.push_back("");
}


void Chunk_Infos::extract_location() {
    while ((_chunk.find("location ", 0) >= 0) && (_chunk[_chunk.find_first_not_of(" \t\v\n\r\f", _chunk.find("location", 0) + 8)] == '/'))
    {
        _location_blocks.push_back(_chunk.substr(_chunk.find("location", 0), _chunk.find_first_of('}', _chunk.find("location", 0)) - _chunk.find("location", 0) + 1));
        //little trick here to stop while loop
        _chunk.erase(_chunk.find("location ", 0), _chunk.find_first_of('}', _chunk.find("location", 0)) - _chunk.find("location", 0) + 1);
    }
    //then reiterate to the same parsing than server
}

void Chunk_Infos::extract_default_error_pages() {
    while (_chunk.find("error_page ", 0) != std::string::npos)
    {
        std::list<std::string> number;
        std::string path;

        std::string error_page_dir = _chunk.substr(_chunk.find("error_page ", 0), _chunk.find_first_of(';', _chunk.find("error_page ", 0)) - _chunk.find("error_page ", 0));
        //little trick here to stop while loop
        _chunk.erase(_chunk.find("error_page ", 0), 1);
        error_page_dir.erase(0, error_page_dir.find_first_of("error_page ") + 11);
        while (error_page_dir.find_first_of(" \t\v\n\r\f", 0) == 0)
            error_page_dir.erase(0, 1);
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
}

void Chunk_Infos::extract_autoindex() {
    if (_chunk.find("autoindex ", 0) != std::string::npos)
    {
        std::string autoindex = _chunk.substr(_chunk.find("autoindex ", 0), _chunk.find_first_of(';', _chunk.find("autoindex ", 0)) - _chunk.find("autoindex ", 0));
        autoindex.erase(0, autoindex.find_first_of("autoindex ") + 10);
        _autoindex = autoindex.substr(autoindex.find_first_not_of("\t\v\n\r\f ", 0), autoindex.find_first_of("\t\v\n\r\f ", autoindex.find_first_not_of("\t\v\n\r\f ", 0)));
        if (_autoindex.compare("on") != 0)
            _autoindex = "off";
    }
    else
        _autoindex = "off";
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

    std::ofstream ofs;
    ofs.open("config_result.txt");
    ofs << _chunk;
}
