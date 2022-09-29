#include "library.hpp"
#include "chunk_infos.hpp"

void Chunk_Infos::extract_host_port(){
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

