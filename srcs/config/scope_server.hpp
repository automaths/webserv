#pragma once

#include "library.hpp"
#include "scope_location.hpp"

class ServerScope {

    public:

    ServerScope();
    ~ServerScope();
    ServerScope(std::string str);
    ServerScope& operator=(ServerScope const &other);
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
    std::string&                                        getRewrite() {return _rewrite; }
    std::string&                                        getRewriteLocation() { return _rewrite_location; }
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
};

