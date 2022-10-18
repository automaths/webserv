#pragma once

#include "library.hpp"
#include "scope_location.hpp"

class LocationScope {

    public:

    LocationScope(){}
    ~LocationScope(){}
    LocationScope(std::string str){
        _chunk = str;
        _has_root = 0;
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
        _directive_types[7] = "limit_upload";
        _directive_types[8] = "rewrite";
        _directive_types[9] = "upload_pass";
        exec[0] = &LocationScope::extract_default_error_pages;
        exec[1] = &LocationScope::extract_client_body_buffer_size;
        exec[2] = &LocationScope::extract_root;
        exec[3] = &LocationScope::extract_allow_method;
        exec[4] = &LocationScope::extract_cgi;
        exec[5] = &LocationScope::extract_index;
        exec[6] = &LocationScope::extract_autoindex;
        exec[7] = &LocationScope::extract_limit_upload;
        exec[8] = &LocationScope::extract_rewrite;
        exec[9] = &LocationScope::extract_upload_pass;
        extract_directives();
        for(std::vector<std::string>::iterator it = _location_blocks.begin(); it != _location_blocks.end(); ++it)
        {
            _locations.push_back(LocationScope(*it));
        }
    }
    LocationScope& operator=(LocationScope const &other) {
        if (this != &other)
        {
            _main_path = other._main_path;
            _locations = other._locations;
            _index = other._index;
            _client_body_buffer_size = other._client_body_buffer_size;
            _autoindex = other._autoindex;
            _limit_upload = other._limit_upload;
            _root = other._root;
            _allow_method = other._allow_method;
            _cgi = other._cgi;
            _default_error_pages = other._default_error_pages;
            _rewrite = other._rewrite;
            _rewrite_location = other._rewrite_location;
            _upload_pass = other._upload_pass;
            _limit_upload = other._limit_upload;
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
    void                                extract_upload_pass(std::string directive);
    void                                extract_rewrite(std::string directive);
    void                                extract_client_body_buffer_size(std::string directive);
    void                                extract_root(std::string directive);
    void                                extract_allow_method(std::string directive);
    void                                extract_cgi(std::string directive);
    void                                extract_index(std::string directive);
    void                                extract_autoindex(std::string directive);
    void                                extract_limit_upload(std::string directive);
    void                                setLocations(std::vector<LocationScope> arg) { _locations = arg; }
    void                                setMainPath(std::string arg) { _main_path = arg; }
    void                                setIndex(std::vector<std::string> arg) { _index = arg; }
    void                                setClientBodyBufferMax(std::string arg) { _client_body_buffer_size = arg; }
    void                                setAutoIndex(std::string arg) { _autoindex = arg; }
    void                                setLimitUpload(std::string arg) { _limit_upload = arg; }
    void                                setRoot(std::string arg) { _root = arg; }
    void                                setAllowMethod(std::vector<std::string> arg) { _allow_method = arg; }
    void                                setCgi(std::map<std::string, std::string> arg) { _cgi = arg; }
    void                                setDefaultErrorPage(std::map<std::string, std::string> arg) { _default_error_pages = arg; }
    std::string&                         getMainPath() { return _main_path; }
    std::vector<LocationScope>&          getLocations() { return _locations; }
    std::vector<std::string>&            getIndex() { return _index; }
    std::string&                         getClientBodyBufferMax() { return _client_body_buffer_size; }
    std::string&                         getAutoIndex() { return _autoindex; }
    std::string&                         getLimitUpload() { return _limit_upload; }
    std::string&                         getRoot() { return _root; }
    std::vector<std::string>&            getAllowMethod() { return _allow_method; }
    std::map<std::string, std::string>&  getCgi() { return _cgi; }
    std::map<std::string, std::string>&  getDefaultErrorPage() { return _default_error_pages; }

    std::string&                         getRootIn() { return _in_root; }

    void                                setMainPathIn(std::string arg) { _in_main_path = arg; }
    void                                setRootIn(std::string arg) { _in_root = arg; }

    int                                 hasRoot() { return _has_root; }

    std::string&                        getRewrite() {return _rewrite_location; }
    std::string&                        getRewriteLocation() { return _rewrite; }
    std::string&                        getUploadPass() { return _upload_pass; }
    void                                setUploadPass(std::string arg) { _upload_pass = arg; } 
    void                                setRewrite(std::string arg) { _rewrite = arg; }
    void                                setRewriteLocation(std::string arg) { _rewrite_location = arg; }

    private:

    std::string                                         _main_path;
    std::string                                         _client_body_buffer_size;
    std::map<std::string, std::string>                  _default_error_pages;
    std::string                                         _root;
    std::vector<std::string>                            _index;
    std::string                                         _autoindex;
    std::string                                         _limit_upload;
    std::vector<std::string>                            _allow_method;
    std::map<std::string, std::string>                  _cgi;
    std::vector<LocationScope>                          _locations;

    std::string                                         _upload_pass;
    std::string                                         _rewrite;
    std::string                                         _rewrite_location;

    std::string                                         _in_main_path;
    std::string                                         _in_root;

    int                                                 _has_root;

    std::string                                         _chunk;
    std::vector<std::string>                            _location_blocks;
    std::vector<std::string>                            _directives;
    std::vector<std::string>                            _configs;
    std::string                                         _directive_types[10];
    void (LocationScope::*exec[10])(std::string str);
};