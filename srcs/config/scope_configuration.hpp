#pragma once

#include "library.hpp"
#include "scope_http.hpp"

class Configuration {

    public:

    Configuration(){}
    ~Configuration(){}
    Configuration(std::string str){
        _file = str;
        check_brackets();
        extract_http_blocks();
        print_configurations();
    }
    //equal operation implementation
    void check_brackets();
    void extract_http_blocks();
    std::string chunking();
    void print_configurations();
    void location_recurs(std::list<LocationScope> location_block, int level);

    std::list<HttpScope>    getHttpScope() { return _http_blocks; }

    private:
    std::list<HttpScope>       _http_blocks;
    std::string                 _file;
    class BracketsException : public std::exception {virtual const char* what() const throw(){return ("Close the brackets in the configuration file please");}};
};
