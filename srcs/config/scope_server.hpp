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
        _directive_types[9] = "limit_upload";
        _directive_types[10] = "rewrite";
        _directive_types[11] = "upload_pass";
        exec[0] = &ServerScope::extract_listen;
        exec[1] = &ServerScope::extract_server_name;
        exec[2] = &ServerScope::extract_default_error_pages;
        exec[3] = &ServerScope::extract_client_body_buffer_size;
        exec[4] = &ServerScope::extract_root;
        exec[5] = &ServerScope::extract_allow_method;
        exec[6] = &ServerScope::extract_cgi;
        exec[7] = &ServerScope::extract_index;
        exec[8] = &ServerScope::extract_autoindex;
        exec[9] = &ServerScope::extract_limit_upload;
        exec[10] = &ServerScope::extract_rewrite;
        exec[11] = &ServerScope::extract_upload_pass;
        extract_directives();
        apply_default();
        for (std::vector<std::string>::iterator it = _location_blocks.begin(); it != _location_blocks.end(); ++it)
        {
            _locations.push_back(LocationScope(*it));
        }
    }
    ServerScope& operator=(ServerScope const &other) {
        if (this != &other)
        {
            _locations = other._locations;
            _address = other._address;
            _listen = other._listen;
            _port = other._port;
            _server_names = other._server_names;
            _index = other._index;
            _client_body_buffer_size = other._client_body_buffer_size;
            _autoindex = other._autoindex;
            _root = other._root;
            _rewrite = other._rewrite;
            _rewrite_location = other._rewrite_location;
            _allow_method = other._allow_method;
            _cgi = other._cgi;
            _default_error_pages = other._default_error_pages;
            _in_root = other._in_root;
            _upload_pass = other._upload_pass;
            _limit_upload = other._limit_upload;

        }
        return *this;
    }
    void                                                clean_comments_header();
    void                                                apply_default();
    void                                                print_result();
    void                                                extract_location_blocks();
    void                                                extract_lines();
    void                                                extract_directives();
    void                                                extract_rules(std::string rule);
    void                                                extract_upload_pass(std::string directive);
    void                                                extract_listen(std::string directive);
    void                                                extract_server_name(std::string directive);
    void                                                extract_default_error_pages(std::string directive);
    void                                                extract_client_body_buffer_size(std::string directive);
    void                                                extract_root(std::string directive);
    void                                                extract_rewrite(std::string directive);
    void                                                extract_allow_method(std::string directive);
    void                                                extract_cgi(std::string directive);
    void                                                extract_index(std::string directive);
    void                                                extract_autoindex(std::string directive);
    void                                                extract_limit_upload(std::string directive);
    void                                                setLocations(std::vector<LocationScope> arg) { _locations = arg; }
    void                                                setAddress(std::string arg) { _address = arg; }
    void                                                setPort(std::string arg) { _port = arg; }
    void                                                setListen(std::map<std::string, std::string> arg) { _listen = arg; }
    void                                                setServerName(std::vector<std::string> arg) { _server_names = arg; }
    void                                                setIndex(std::vector<std::string> arg) { _index = arg; }
    void                                                setClientBodyBufferMax(std::string arg) {_client_body_buffer_size = arg; }
    void                                                setAutoIndex(std::string arg) { _autoindex = arg; }
    void                                                setLimitUpload(std::string arg) { _limit_upload = arg; }
    void                                                setRoot(std::string arg) { _root = arg; }
    void                                                setAllowMethod(std::vector<std::string> arg) { _allow_method = arg; }
    void                                                setCgi(std::map<std::string, std::string> arg) { _cgi = arg; }
    void                                                setDefaultErrorPage(std::map<std::string, std::string> arg) { _default_error_pages = arg; }
    std::vector<LocationScope>&                         getLocations() { return _locations; }
    std::string&                                        getAddress() { return _address; }
    std::string&                                        getPort() { return _port; }
    std::vector<std::string>&                           getServerName() { return _server_names; }
    std::vector<std::string>&                           getIndex() { return _index; }
    std::string&                                        getClientBodyBufferMax() { return _client_body_buffer_size; }
    std::string&                                        getAutoIndex() { return _autoindex; }
    std::string&                                        getLimitUpload() { return _autoindex; }
    std::string&                                        getRoot() { return _root; }
    std::vector<std::string>&                           getAllowMethod() { return _allow_method; }
    std::map<std::string, std::string>&                 getCgi() { return _cgi; }
    std::map<std::string, std::string>&                 getDefaultErrorPage() { return _default_error_pages; }
    std::map<std::string, std::string>&                 getListen() { return _listen; }
    void                                                setRootIn(std::string arg) { _in_root = arg; }

    std::string&                                        getRewrite() {return _rewrite_location; }
    std::string&                                        getRewriteLocation() { return _rewrite; }
    std::string&                                        getUploadPass() { return _upload_pass; }
    void                                                setUploadPass(std::string arg) { _upload_pass = arg; } 
    void                                                setRewrite(std::string arg) { _rewrite = arg; }
    void                                                setRewriteLocation(std::string arg) { _rewrite_location = arg; }

    private:

    std::vector<LocationScope>                          _locations;
    std::string                                         _address;
    std::string                                         _port;
    std::map<std::string, std::string>                  _listen;

    std::vector<std::string>                            _server_names;
    std::vector<std::string>                            _index;
    std::string                                         _client_body_buffer_size;
    std::string                                         _autoindex;
    std::string                                         _root;
    std::vector<std::string>                            _allow_method;
    std::map<std::string, std::string>                  _cgi;
    std::map<std::string, std::string>                  _default_error_pages;

    std::string                                         _limit_upload;
    std::string                                         _rewrite;
    std::string                                         _rewrite_location;
    std::string                                         _upload_pass;

    std::string                                         _in_root;

    std::string                                         _chunk;
    std::vector<std::string>                            _location_blocks;
    std::vector<std::string>                            _directives;
    std::vector<std::string>                            _configs;
    std::string                                         _directive_types[12];
    void (ServerScope::*exec[12])(std::string str);

    //forbid upload
};

