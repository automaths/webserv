#pragma once

#include "library.hpp"

class Chunk_Infos {

    public:

    Chunk_Infos(){}
    ~Chunk_Infos(){}

    Chunk_Infos(std::string str){
        _chunk = str;
        extract_address_port();
        extract_server_name();
        extract_default_error_pages();
        extract_client_body_buffer_size();
        extract_root();
        extract_location();

        // print_infos();
    }

    void extract_address_port();
    void extract_server_name();
    void extract_default_error_pages();
    void extract_client_body_buffer_size();
    void extract_root();

    // void extract_add_header() {}

    void extract_location() {

        std::list<std::string> location_blocks;
        while ((_chunk.find("location ", 0) >= 0) && (_chunk[_chunk.find_first_not_of(" \t\v\n\r\f", _chunk.find("location", 0) + 8)] == '/'))
        {
            location_blocks.push_back(_chunk.substr(_chunk.find("location", 0), _chunk.find_first_of('}', _chunk.find("location", 0)) - _chunk.find("location", 0) + 1));
            //little trick here to stop while loop
            _chunk.erase(_chunk.find("location ", 0), 1);
        }
        //then reiterate to the same parsing than server
    }








    void print_infos() {
        std::cout << "the address is: " << _address << std::endl;
        std::cout << "the port is: " << _port << std::endl;
        for (std::list<std::string>::iterator it = _server_names.begin(); it != _server_names.end(); ++it)
            std::cout << "server_name: " << *it << std::endl;
        for (std::list<std::pair<std::list<std::string>, std::string> >::iterator it = _default_error_pages.begin(); it != _default_error_pages.end(); ++it)
        {
            std::cout << "error pages: " << std::endl;
            for (std::list<std::string>::iterator ite = it->first.begin(); ite != it->first.end(); ++ite)
                std::cout << "  error number: " << *ite << std::endl;
            std::cout << "  path: " << it->second << std::endl;
        }
        std::cout << "body max: " << _client_body_buffer_size << std::endl;
        std::cout << "root: " << _root << std::endl;
    }

    private:

    std::string _chunk;

    std::string _address;
    std::string _port;
    std::list<std::string> _server_names;
    std::list<std::pair<std::list<std::string>, std::string> > _default_error_pages;
    std::string _client_body_buffer_size;
    std::string _root;


    
    // std::list<Location_Infos> locations;
    // std::list<std::string> cgi;

    // bool is_http;
    // bool has_server_names;
};

