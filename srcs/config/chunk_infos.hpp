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
        extract_default_error_pages();

        print_infos();
    }

    void extract_default_error_pages() {
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


    void print_infos() {
        std::cout << "the address is: " << _address << std::endl;
        std::cout << "the port is: " << _port << std::endl;
        for (std::list<std::string>::iterator it = _server_names.begin(); it != _server_names.end(); ++it)
            std::cout << "Server_name : " << *it << std::endl;
        for (std::list<std::pair<std::list<std::string>, std::string> >::iterator it = _default_error_pages.begin(); it != _default_error_pages.end(); ++it)
        {
            std::cout << "Error pages : " << std::endl;
            for (std::list<std::string>::iterator ite = it->first.begin(); ite != it->first.end(); ++ite)
                std::cout << "  Error number: " << *ite << std::endl;
            std::cout << "  Path: " << it->second << std::endl;
        }
    }

    void extract_host_port();
    void extract_server_name();

    private:

    std::string _chunk;

    std::string _address;
    std::string _port;
    std::list<std::string> _server_names;
    std::list<std::pair<std::list<std::string>, std::string> > _default_error_pages;
    // std::string client_body_max_size;
    // std::list<Location_Infos> locations;
    // std::list<std::string> cgi;

    // bool is_http;
    // bool has_server_names;
};

