#pragma once

#include "library.hpp"

class Chunk_Infos {

    public:

    Chunk_Infos(){}
    ~Chunk_Infos(){}

    Chunk_Infos(std::string str){
        _chunk = str;
        extract_host_port();
        extract_server_name();

        print_infos();
    }

    void extract_server_name() {
        if (_chunk.find("server_name ", 0) != std::string::npos)
        {
            std::string server_name_dir = _chunk.substr(_chunk.find("server_name ", 0), _chunk.find_first_of(';', _chunk.find("server_name ", 0)) - _chunk.find("server_name ", 0));
            server_name_dir.erase(0, server_name_dir.find_first_of("server_name ") + 12);
            while (server_name_dir.find_first_of(" \t\v\n\r\f", 0) == 0)
                server_name_dir.erase(0, 1);
            std::cout << "the server_name directive is: " << server_name_dir << std::endl;
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

        }

        // if (server_name_dir.find_first_of(':', 0) != std::string::npos)
        // {
        //     _address = server_name_dir.substr(0, server_name_dir.find_first_of(':', 0));
        //     server_name_dir.erase(0, server_name_dir.find_first_of(':', 0) + 1);
        //     _port = server_name_dir;
        // }
        // else
        // {
        //     if (server_name_dir.find_first_not_of("\t\v\n\r\f 0123456789") == std::string::npos)
        //     {
        //         _port = server_name_dir;
        //         _address = "*";
        //     }
        //     else
        //     {
        //         _port = "80";
        //         _address = server_name_dir;
        //     }
        // }

    }

    void print_infos() {
        std::cout << "the address is: " << _address << std::endl;
        std::cout << "the port is: " << _port << std::endl;
    }
    void extract_host_port();

    private:

    std::string _chunk;

    std::string _address;
    std::string _port;
    std::list<std::string> _server_names;
    // std::list<std::string> default_error_pages;
    // std::string client_body_max_size;
    // std::list<Location_Infos> locations;
    // std::list<std::string> cgi;

    // bool is_http;
    // bool has_server_names;
};

