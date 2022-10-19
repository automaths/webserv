#pragma once

#include "library.hpp"
#include "scope_http.hpp"
#include "scope_server.hpp"

class Configuration {

    public:

    Configuration();
    ~Configuration();
    Configuration(std::string str);
    Configuration&                              operator=(Configuration const &other);
    void                                        the_inheritance();
    void                                        the_recursivity(std::vector<LocationScope> &locations);
    void                                        transfer_servers();
    void                                        print_resultings();
    void                                        location_recursivity(std::vector<LocationScope> location_block, int level);
    void                                        check_brackets();
    void                                        extract_http_blocks();
    void                                        print_configurations();
    void                                        location_recurs(std::vector<LocationScope> location_block, int level);
    std::string                                 chunking();
    std::vector<ServerScope>                    getServers();
    std::vector<HttpScope>                      getHttpScope();

    private:

    std::vector<HttpScope>                      _http_blocks;
    std::vector<ServerScope>                    _server_blocks;
    std::string                                 _file;
    class BracketsException : public std::exception {virtual const char* what() const throw(){return ("Close the brackets in the configuration file please");}};
};
