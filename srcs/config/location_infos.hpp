#pragma once

#include "library.hpp"

#include "library.hpp"
#include "location_infos.hpp"

class LocationInfos {

    public:

    LocationInfos(){}
    ~LocationInfos(){}
    LocationInfos(std::string str){
        _chunk = str;
        extract_main_path();
        extract_location_blocks();
        extract_lines();
        _directive_types[0] = "error_page";
        _directive_types[1] = "client_body_buffer_size";
        _directive_types[2] = "root";
        _directive_types[3] = "allow_method";
        _directive_types[4] = "cgi";
        _directive_types[5] = "index";
        _directive_types[6] = "autoindex";
        _directive_types[7] = "try_files";
        exec[0] = &LocationInfos::extract_default_error_pages;
        exec[1] = &LocationInfos::extract_client_body_buffer_size;
        exec[2] = &LocationInfos::extract_root;
        exec[3] = &LocationInfos::extract_allow_method;
        exec[4] = &LocationInfos::extract_cgi;
        exec[5] = &LocationInfos::extract_index;
        exec[6] = &LocationInfos::extract_autoindex;
        exec[7] = &LocationInfos::extract_try_files;
        extract_directives();
        apply_default();
        while (_location_blocks.size() != 0)
        {
            _locations.push_back(LocationInfos(_location_blocks.front()));
            _location_blocks.pop_front();
        }
        // print_result();
    }

    void extract_location_blocks();
    void extract_lines();
    void extract_directives();
    void extract_main_path(); 
    void extract_rules(std::string rule);
    void apply_default();
    void print_result();

    void extract_default_error_pages(std::string directive);
    void extract_client_body_buffer_size(std::string directive);
    void extract_root(std::string directive);
    void extract_allow_method(std::string directive);
    void extract_cgi(std::string directive);
    void extract_index(std::string directive);
    void extract_autoindex(std::string directive);
    void extract_try_files(std::string directive);

    private:

    std::string                                         _main_path;

    std::string                                         _client_body_buffer_size;
    std::map<std::string, std::string>                  _default_error_pages;
    std::list<LocationInfos>                            _locations;
    std::string                                         _root;
    std::list<std::string>                              _try_files;
    std::list<std::string>                              _index;
    std::string                                         _autoindex;
    std::list<std::string>                              _allow_method;
    std::list<std::pair<std::string, std::string> >     _cgi;


    std::string                                         _chunk;
    std::list<std::string>                              _location_blocks;
    std::list<std::string>                              _directives;


    std::list<std::string>                              _configs;

    std::string                                         _directive_types[8];
    void (LocationInfos::*exec[8])(std::string str);
};