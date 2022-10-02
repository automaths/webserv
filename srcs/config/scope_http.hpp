#pragma once

#include "library.hpp"
#include "scope_server.hpp"
#include "scope_location.hpp"

class HttpScope {
    public:

    HttpScope(){}
    ~HttpScope(){}

    void extract_server_blocks();
    void extract_lines();
    void clean_http_header();
    void extract_directives();
    void extract_rules(std::string rule);
    void apply_default();

    void extract_default_error_pages(std::string directive);
    void extract_client_body_buffer_size(std::string directive);
    void extract_root(std::string directive);
    void extract_cgi(std::string directive);
    void extract_index(std::string directive);
    void extract_autoindex(std::string directive);

    void print_result();

    HttpScope(std::string str) {
        _block = str;
        clean_http_header();
        extract_server_blocks();
        extract_lines();
    
        _directive_types[0] = "error_page";
        _directive_types[1] = "client_body_buffer_size";
        _directive_types[2] = "root";
        _directive_types[3] = "cgi";
        _directive_types[4] = "index";
        _directive_types[5] = "autoindex";
        exec[0] = &HttpScope::extract_default_error_pages;
        exec[1] = &HttpScope::extract_client_body_buffer_size;
        exec[2] = &HttpScope::extract_root;
        exec[3] = &HttpScope::extract_cgi;
        exec[4] = &HttpScope::extract_index;
        exec[5] = &HttpScope::extract_autoindex;
        extract_directives();
        //maybe after all the parsing is done, to be considered
        apply_default();
        for (std::list<std::string>::iterator it = _server_blocks.begin(); it != _server_blocks.end(); ++it)
            _servers.push_back(ServerScope(*it));
        print_result();
    }

    private:

    std::string                                         _block;
    std::list<std::string>                              _server_blocks;
    std::list<ServerScope>                               _servers;
    std::list<std::string>                              _directives;

    std::string                                         _client_body_buffer_size;
    std::map<std::string, std::string>                  _default_error_pages;
    std::string                                         _root;
    std::list<std::string>                              _index;
    std::string                                         _autoindex;
    std::map<std::string, std::string>                  _cgi;
    std::string                                         _directive_types[6];
    void (HttpScope::*exec[6])(std::string str);
};