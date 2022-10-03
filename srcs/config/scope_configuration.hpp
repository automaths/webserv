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

        apply_inheritance();

        print_configurations();
    }

    void exec(){
        // apply_inheritance();
        // print_configurations();       
    }

    void apply_inheritance(){

        for (std::list<HttpScope>::iterator it_one = _http_blocks.begin(); it_one != _http_blocks.end(); ++it_one)
        {
            // ofs << "client_body_buffer_size: " << it_one->getClientBodyBufferMax() << std::endl;
            // ofs << "default error page " << it_two->first << " associated to path " << it_two->second << std::endl;
            // ofs << "index: " << *it_two << std::endl;
            // ofs << "autoindex: " << it_one->getAutoIndex() << std::endl;
            // ofs << "root: " << it_one->getRoot() << std::endl;
            // ofs << "cgi: extension " << it_two->first << " associated to path " << it_two->second << std::endl;

            std::list<ServerScope> getServers = it_one->getServers();
            for (std::list<ServerScope>::iterator it_two = getServers.begin(); it_two != getServers.end(); ++it_two)
            {






                // std::map<std::string, std::string> getDefaultErrorPage = it_two->getDefaultErrorPage(); //to avoid dangling pointer warning
                // for (std::map<std::string, std::string>::iterator it_three = getDefaultErrorPage.begin(); it_three != getDefaultErrorPage.end(); ++it_three)
                //     ofs << "    default error page " << it_three->first << " associated to path " << it_three->second << std::endl;
                // std::list<std::string> getIndex = it_two->getIndex();
                // for (std::list<std::string>::iterator it_three = getIndex.begin(); it_three != getIndex.end(); ++it_three)
                //     ofs << "    index: " << *it_three << std::endl;
                // ofs << "    autoindex: " << it_two->getAutoIndex() << std::endl;
                // ofs << "    root: " << it_two->getRoot() << std::endl;
                // std::map<std::string, std::string> getCgi = it_two->getCgi();
                // for (std::map<std::string, std::string>::iterator it_three = getCgi.begin(); it_three != getCgi.end(); ++it_three)
                //     ofs << "    cgi: extension " << it_three->first << " associated to path " << it_three->second << std::endl;
                // if (it_two->getLocations().size() > 0)
                //     location_recurs(it_two->getLocations(), 1);
            }
        }
    }




    //equal operation implementation
    void check_brackets();
    void extract_http_blocks();
    std::string chunking();
    void print_configurations();
    void location_recurs(std::list<LocationScope> location_block, int level);

    std::list<HttpScope>&    getHttpScope() {return _http_blocks;}

    private:
    std::list<HttpScope>       _http_blocks;

    std::string                 _file;
    class BracketsException : public std::exception {virtual const char* what() const throw(){return ("Close the brackets in the configuration file please");}};
};
