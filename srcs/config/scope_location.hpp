#pragma once

#include "library.hpp"
#include "scope_location.hpp"

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
        print_result();
    }
    LocationInfos& operator=(LocationInfos &other) {
        if (this != &other)
        {
            _main_path = other._main_path;
            // _locations = other._locations;
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
    void                                extract_main_path(); 
    void                                extract_rules(std::string rule);
    void                                apply_default();
    void                                print_result();
    void                                extract_default_error_pages(std::string directive);
    void                                extract_client_body_buffer_size(std::string directive);
    void                                extract_root(std::string directive);
    void                                extract_allow_method(std::string directive);
    void                                extract_cgi(std::string directive);
    void                                extract_index(std::string directive);
    void                                extract_autoindex(std::string directive);
    void                                extract_try_files(std::string directive);
    // void                                setLocations(std::list<LocationInfos> arg) { _locations = arg; }
    void                                setMainPath(std::string arg) { _main_path = arg; }
    void                                setTryFiles(std::list<std::string> arg) { _try_files = arg; }
    void                                setIndex(std::list<std::string> arg) { _index = arg; }
    void                                setClientBodyBufferMax(std::string arg) { _client_body_buffer_size = arg; }
    void                                setAutoIndex(std::string arg) { _autoindex = arg; }
    void                                setRoot(std::string arg) { _root = arg; }
    void                                setAllowMethod(std::list<std::string> arg) { _allow_method = arg; }
    void                                getCgi(std::map<std::string, std::string> arg) { _cgi = arg; }
    void                                getDefaultErrorPage(std::map<std::string, std::string> arg) { _default_error_pages = arg; }
    std::string                         getMainPath() { return _main_path; }
    std::list<LocationInfos>            getLocations() { return _locations; }
    std::list<std::string>              getTryFiles() { return _try_files; }
    std::list<std::string>              getIndex() { return _index; }
    std::string                         getClientBodyBufferMax() { return _client_body_buffer_size; }
    std::string                         getAutoIndex() { return _autoindex; }
    std::string                         getRoot() { return _root; }
    std::list<std::string>              getAllowMethod() { return _allow_method; }
    std::map<std::string, std::string>  getCgi() { return _cgi; }
    std::map<std::string, std::string>  getDefaultErrorPage() { return _default_error_pages; }

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
    std::map<std::string, std::string>                  _cgi;

    std::string                                         _chunk;
    std::list<std::string>                              _location_blocks;
    std::list<std::string>                              _directives;
    std::list<std::string>                              _configs;
    std::string                                         _directive_types[8];
    void (LocationInfos::*exec[8])(std::string str);
};