#pragma once

#include "library.hpp"
#include "scope_location.hpp"

class ServerScope {

    public:

    ServerScope(){}
    ~ServerScope(){}
    ServerScope(std::string str){
        _chunk = str;
        clean_comments_header();
        extract_location_blocks();
        extract_lines();
        _directive_types[0] = "listen";
        _directive_types[1] = "server_name";
        _directive_types[2] = "error_page";
        _directive_types[3] = "client_body_buffer_size";
        _directive_types[4] = "root";
        _directive_types[5] = "allow_method";
        _directive_types[6] = "cgi";
        _directive_types[7] = "index";
        _directive_types[8] = "autoindex";
        _directive_types[9] = "try_files";
        exec[0] = &ServerScope::extract_listen;
        exec[1] = &ServerScope::extract_server_name;
        exec[2] = &ServerScope::extract_default_error_pages;
        exec[3] = &ServerScope::extract_client_body_buffer_size;
        exec[4] = &ServerScope::extract_root;
        exec[5] = &ServerScope::extract_allow_method;
        exec[6] = &ServerScope::extract_cgi;
        exec[7] = &ServerScope::extract_index;
        exec[8] = &ServerScope::extract_autoindex;
        exec[9] = &ServerScope::extract_try_files;
        extract_directives();
        apply_default();
        for (std::list<std::string>::iterator it = _location_blocks.begin(); it != _location_blocks.end(); ++it)
            _locations.push_back(LocationScope(*it));
    }
    void inheritance(){
        if (_client_body_buffer_size.size() == 0)
            _client_body_buffer_size = _in_client_body_buffer_size;
        if (_index.size() == 0)
            _index = _in_index;
        if (_autoindex.size() == 0)
            _autoindex = _in_autoindex;
        if (_root.size() == 0)
            _root = _in_root;
        if (_cgi.size() == 0)
            _cgi = _in_cgi;
        if (_default_error_pages.size() == 0)
            _default_error_pages = _in_default_error_pages;
    }
    ServerScope& operator=(ServerScope &other) {
        if (this != &other)
        {
            _locations = other._locations;
            _address = other._address;
            _port = other._port;
            _server_names = other._server_names;
            _try_files = other._try_files;
            _index = other._index;
            _client_body_buffer_size = other._client_body_buffer_size;
            _autoindex = other._autoindex;
            _root = other._root;
            _allow_method = other._allow_method;
            _cgi = other._cgi;
            _default_error_pages = other._default_error_pages;
        }
        return *this;
    }
    void                                extract_location_blocks();
    void                                extract_lines();
    void                                extract_directives();
    void                                clean_comments_header();
    void                                extract_rules(std::string rule);
    void                                apply_default();
    void                                print_result();
    void                                extract_listen(std::string directive);
    void                                extract_server_name(std::string directive);
    void                                extract_default_error_pages(std::string directive);
    void                                extract_client_body_buffer_size(std::string directive);
    void                                extract_root(std::string directive);
    void                                extract_allow_method(std::string directive);
    void                                extract_cgi(std::string directive);
    void                                extract_index(std::string directive);
    void                                extract_autoindex(std::string directive);
    void                                extract_try_files(std::string directive);
    void                                setLocations(std::list<LocationScope> arg) { _locations = arg; }
    void                                setAddress(std::string arg) { _address = arg; }
    void                                setPort(std::string arg) { _port = arg; }
    void                                setServerName(std::list<std::string> arg) { _server_names = arg; }
    void                                setTryFiles(std::list<std::string> arg) { _try_files = arg; }
    void                                setIndex(std::list<std::string> arg) { _index = arg; }
    void                                setClientBodyBufferMax(std::string arg) {_client_body_buffer_size = arg; }
    void                                setAutoIndex(std::string arg) { _autoindex = arg; }
    void                                setRoot(std::string arg) { _root = arg; }
    void                                setAllowMethod(std::list<std::string> arg) { _allow_method = arg; }
    void                                setCgi(std::map<std::string, std::string> arg) { _cgi = arg; }
    void                                setDefaultErrorPage(std::map<std::string, std::string> arg) { _default_error_pages = arg; }
    std::list<LocationScope>            getLocations() { return _locations; }
    std::string                         getAddress() { return _address; }
    std::string                         getPort() { return _port; }
    std::list<std::string>              getServerName() { return _server_names; }
    std::list<std::string>              getTryFiles() { return _try_files; }
    std::list<std::string>              getIndex() { return _index; }
    std::string&                        getClientBodyBufferMax() { return _client_body_buffer_size; }
    std::string                         getAutoIndex() { return _autoindex; }
    std::string                         getRoot() { return _root; }
    std::list<std::string>              getAllowMethod() { return _allow_method; }
    std::map<std::string, std::string>  getCgi() { return _cgi; }
    std::map<std::string, std::string>  getDefaultErrorPage() { return _default_error_pages; }
    void                                setClientBodyBufferMaxIn(std::string arg) { _in_client_body_buffer_size = arg; }
    void                                setIndexIn(std::list<std::string> arg) { _in_index = arg; }
    void                                setAutoIndexIn(std::string arg) { _in_autoindex = arg; }
    void                                setRootIn(std::string arg) { _in_root = arg; }
    void                                setCgiIn(std::map<std::string, std::string> arg) { _in_cgi = arg; }
    void                                setDefaultErrorPageIn(std::map<std::string, std::string> arg) { _in_default_error_pages = arg; }

    private:

    std::list<LocationScope>                            _locations;
    std::string                                         _address;
    std::string                                         _port;
    std::list<std::string>                              _server_names;
    std::list<std::string>                              _try_files;
    std::list<std::string>                              _index;
    std::string                                         _client_body_buffer_size;
    std::string                                         _autoindex;
    std::string                                         _root;
    std::list<std::string>                              _allow_method;
    std::map<std::string, std::string>                  _cgi;
    std::map<std::string, std::string>                  _default_error_pages;



    std::string                                         _in_client_body_buffer_size;
    std::map<std::string, std::string>                  _in_default_error_pages;
    std::string                                         _in_root;
    std::list<std::string>                              _in_index;
    std::string                                         _in_autoindex;
    std::map<std::string, std::string>                  _in_cgi;

    std::string                                         _chunk;
    std::list<std::string>                              _location_blocks;
    std::list<std::string>                              _directives;
    std::list<std::string>                              _configs;
    std::string                                         _directive_types[10];
    void (ServerScope::*exec[10])(std::string str);
};

