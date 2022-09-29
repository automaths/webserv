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
    {
        _server_names.push_back("");
    }
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
        _default_error_pages.push_back(std::make_pair(number, path));
    }
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
