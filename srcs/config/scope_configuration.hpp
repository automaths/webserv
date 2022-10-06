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
        transfer_servers();
        the_inheritance();
        print_resultings();
    }
    Configuration& operator=(Configuration const &other){
        if (this != &other)
            _http_blocks = other._http_blocks;
        return *this;
    }
    void the_inheritance() {

        for (std::vector<ServerScope>::iterator it = _server_blocks.begin(); it != _server_blocks.end(); ++it)
        {
            std::vector<LocationScope> &getLocations = it->getLocations();
            for (std::vector<LocationScope>::iterator it_one = getLocations.begin(); it_one != getLocations.end(); ++it_one)
            {
                it_one->getRootIn() = it->getRoot();
                if (it_one->getIndex().size() == 0)
                    it_one->getIndex() = it->getIndex();
                if (it_one->getClientBodyBufferMax().size() == 0)
                    it_one->getClientBodyBufferMax() = it->getClientBodyBufferMax();
                if (it_one->getAutoIndex().size() == 0)
                    it_one->getAutoIndex() = it->getAutoIndex();
                if (it_one->getRoot().size() == 0)
                    it_one->getRoot() = it_one->getRootIn() + it_one->getMainPath().substr(it_one->getMainPath().find_last_of('/'), it_one->getMainPath().size() - it_one->getMainPath().find_last_of('/'));
                //there can be duplicates in the allow method
                std::vector<std::string> getAllowMethod = it->getAllowMethod(); 
                for (std::vector<std::string>::iterator it = getAllowMethod.begin(); it != getAllowMethod.end(); ++it)
                    it_one->getAllowMethod().push_back(*it);
                std::map<std::string, std::string> getCgi = it->getCgi();
                for (std::map<std::string, std::string>::iterator it = getCgi.begin(); it != getCgi.end(); ++it)
                    it_one->getCgi()[it->first] = it->second;
                std::map<std::string, std::string> getDefaultErrorPage = it->getDefaultErrorPage();
                for (std::map<std::string, std::string>::iterator it = getDefaultErrorPage.begin(); it != getDefaultErrorPage.end(); ++it)
                    it_one->getDefaultErrorPage()[it->first] = it->second;
            }
            if (it->getLocations().size())
                the_recursivity(it->getLocations());
        }
    }
    void the_recursivity(std::vector<LocationScope> &locations) {
        for (std::vector<LocationScope>::iterator it = locations.begin(); it != locations.end(); ++it)
        {
            std::vector<LocationScope> &getLocations = it->getLocations();
            for (std::vector<LocationScope>::iterator it_one = getLocations.begin(); it_one != getLocations.end(); ++it_one)
            {
                it_one->getRootIn() = it->getRoot();
                if (it_one->getIndex().size() == 0)
                    it_one->getIndex() = it->getIndex();
                if (it_one->getClientBodyBufferMax().size() == 0)
                    it_one->getClientBodyBufferMax() = it->getClientBodyBufferMax();
                if (it_one->getAutoIndex().size() == 0)
                    it_one->getAutoIndex() = it->getAutoIndex();
                if (it_one->getRoot().size() == 0)
                    it_one->getRoot() = it_one->getRootIn() + it_one->getMainPath().substr(it_one->getMainPath().find_last_of('/'), it_one->getMainPath().size() - it_one->getMainPath().find_last_of('/'));
                std::vector<std::string> getAllowMethod = it->getAllowMethod(); 
                for (std::vector<std::string>::iterator it = getAllowMethod.begin(); it != getAllowMethod.end(); ++it)
                    it_one->getAllowMethod().push_back(*it);
                std::map<std::string, std::string> getCgi = it->getCgi();
                for (std::map<std::string, std::string>::iterator it = getCgi.begin(); it != getCgi.end(); ++it)
                    it_one->getCgi()[it->first] = it->second;
                std::map<std::string, std::string> getDefaultErrorPage = it->getDefaultErrorPage();
                for (std::map<std::string, std::string>::iterator it = getDefaultErrorPage.begin(); it != getDefaultErrorPage.end(); ++it)
                    it_one->getDefaultErrorPage()[it->first] = it->second;
            }
            if (it->getLocations().size())
                the_recursivity(it->getLocations());
        }
    }
    std::vector<ServerScope>    getServers() {return _server_blocks;}

    void transfer_servers();
    void print_resultings();
    void location_recursivity(std::vector<LocationScope> location_block, int level);
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
