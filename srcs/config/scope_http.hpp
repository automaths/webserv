#pragma once

#include "library.hpp"
#include "scope_server.hpp"
#include "scope_location.hpp"

class HttpScope {

    public:

    HttpScope(){}
    ~HttpScope(){}
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
        apply_default();
        for (std::vector<std::string>::iterator it = _server_blocks.begin(); it != _server_blocks.end(); ++it)
            _servers.push_back(ServerScope(*it));
    }
    HttpScope& operator=(HttpScope const &other){
        if (this != &other)
        {
            _servers = other._servers;
            _client_body_buffer_size = other._client_body_buffer_size;
            _default_error_pages = other._default_error_pages;
            _root = other._root;
            _index = other._index;
            _autoindex = other._autoindex;
            _cgi = other._cgi;
        }
        return *this;
    }
    void                                                clean_http_header();
    void                                                apply_default();
    void                                                print_result();
    void                                                extract_server_blocks();
    void                                                extract_lines();
    void                                                extract_directives();
    void                                                extract_rules(std::string rule);
    void                                                extract_default_error_pages(std::string directive);
    void                                                extract_client_body_buffer_size(std::string directive);
    void                                                extract_root(std::string directive);
    void                                                extract_cgi(std::string directive);
    void                                                extract_index(std::string directive);
    void                                                extract_autoindex(std::string directive);
    void                                                setIndex(std::vector<std::string> arg) { _index = arg; }
    void                                                setClientBodyBufferMax(std::string arg) { _client_body_buffer_size = arg; }
    void                                                setAutoIndex(std::string arg) { _autoindex = arg; }
    void                                                setRoot(std::string arg) { _root = arg; }
    void                                                setCgi(std::map<std::string, std::string> arg) { _cgi = arg; }
    void                                                settDefaultErrorPage(std::map<std::string, std::string> arg) { _default_error_pages = arg; }
    std::vector<std::string>&                           getIndex() { return _index; }
    std::string&                                        getClientBodyBufferMax() { return _client_body_buffer_size; }
    std::string&                                        getAutoIndex() { return _autoindex; }
    std::string&                                        getRoot() { return _root; }
    std::map<std::string, std::string>&                 getCgi() { return _cgi; }
    std::map<std::string, std::string>&                 getDefaultErrorPage() { return _default_error_pages; }
    std::vector<ServerScope>&                           getServers(){ return _servers; }

    private:

    std::vector<ServerScope>                            _servers;
    std::string                                         _client_body_buffer_size;
    std::map<std::string, std::string>                  _default_error_pages;
    std::string                                         _root;
    std::vector<std::string>                            _index;
    std::string                                         _autoindex;
    std::map<std::string, std::string>                  _cgi;

    std::string                                         _block;
    std::vector<std::string>                            _server_blocks;
    std::vector<std::string>                            _directives;
    std::string                                         _directive_types[6];
    void (HttpScope::*exec[6])(std::string str);
};