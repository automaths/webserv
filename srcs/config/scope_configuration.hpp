#pragma once

#include "library.hpp"
#include "scope_http.hpp"
#include "scope_server.hpp"

class Configuration {

    public:

    Configuration(){}
    ~Configuration(){}
    Configuration(std::string str){
        _file = str;
        check_brackets();
        extract_http_blocks();
        print_configurations();
        transfer_servers();
    }
    Configuration& operator=(Configuration const &other){
        if (this != &other)
            _http_blocks = other._http_blocks;
        return *this;
    }
    void transfer_servers(){

        for (std::vector<HttpScope>::iterator it_one = _http_blocks.begin(); it_one != _http_blocks.end(); ++it_one)
        {
            std::vector<ServerScope> getServers = it_one->getServers();
            for (std::vector<ServerScope>::iterator it_two = getServers.begin(); it_two != getServers.end(); ++it_two)
                _server_blocks.push_back(*it_two);
        }
    }

    std::vector<ServerScope>    getServers() {return _server_blocks;}

    void check_brackets();
    void extract_http_blocks();
    std::string chunking();
    void print_configurations();
    void location_recurs(std::vector<LocationScope> location_block, int level);
    std::vector<HttpScope>    getHttpScope() { return _http_blocks; }

    private:

    std::vector<HttpScope>      _http_blocks;
    std::vector<ServerScope>      _server_blocks;
    std::string                 _file;
    class BracketsException : public std::exception {virtual const char* what() const throw(){return ("Close the brackets in the configuration file please");}};
};
