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

    void print_configurations() {

        std::ofstream ofs;
        ofs.open("./configurations/parsed.txt");
        std::ofstream ofss;
        ofss.open("./configurations/location_parsed.txt");

        ofs << "------------| PRINTING THE CONFIGURATION FILE |------------" << std::endl;
        
        for (std::list<HttpScope>::iterator it_one = _http_blocks.begin(); it_one != _http_blocks.end(); ++it_one)
        {
            ofs << "\n     ---------------| THE HTTP SCOPE |---------------        \n" << std::endl;

            ofs << "client_body_buffer_size: " << it_one->getClientBodyBufferMax() << std::endl;
            std::map<std::string, std::string> getDefaultErrorPage = it_one->getDefaultErrorPage(); //to avoid dangling pointer warning
            for (std::map<std::string, std::string>::iterator it_two = getDefaultErrorPage.begin(); it_two != getDefaultErrorPage.end(); ++it_two)
                ofs << "default error page " << it_two->first << " associated to path " << it_two->second << std::endl;
            std::list<std::string> getIndex = it_one->getIndex();
            for (std::list<std::string>::iterator it_two = getIndex.begin(); it_two != getIndex.end(); ++it_two)
                ofs << "index: " << *it_two << std::endl;
            ofs << "autoindex: " << it_one->getAutoIndex() << std::endl;
            ofs << "root: " << it_one->getRoot() << std::endl;
            std::map<std::string, std::string> getCgi = it_one->getCgi();
            for (std::map<std::string, std::string>::iterator it_two = getCgi.begin(); it_two != getCgi.end(); ++it_two)
                ofs << "cgi: extension " << it_two->first << " associated to path " << it_two->second << std::endl;
            std::list<ServerScope> getServers = it_one->getServers();
            for (std::list<ServerScope>::iterator it_two = getServers.begin(); it_two != getServers.end(); ++it_two)
            {
                ofs << "\n            -------| THE SERVER SCOPE |-------            \n" << std::endl;

                ofs << "    address: " << it_two->getAddress() << std::endl;
                ofs << "    port: " << it_two->getPort() << std::endl;
                std::list<std::string> getServerName = it_two->getServerName();
                for (std::list<std::string>::iterator it_three = getServerName.begin(); it_three != getServerName.end(); ++it_three)
                    ofs << "    server_name: " << *it_three << std::endl;
                std::list<std::string> getTryFiles = it_two->getTryFiles();
                for (std::list<std::string>::iterator it_three = getTryFiles.begin(); it_three != getTryFiles.end(); ++it_three)
                    ofs << "    try_files: " << *it_three << std::endl;
                std::list<std::string> getAllowMethod = it_two->getAllowMethod();
                for (std::list<std::string>::iterator it_three = getAllowMethod.begin(); it_three != getAllowMethod.end(); ++it_three)
                    ofs << "    allow_method: " << *it_three << std::endl;
                ofs << "    client_body_buffer_size: " << it_two->getClientBodyBufferMax() << std::endl;
                std::map<std::string, std::string> getDefaultErrorPage = it_two->getDefaultErrorPage(); //to avoid dangling pointer warning
                for (std::map<std::string, std::string>::iterator it_three = getDefaultErrorPage.begin(); it_three != getDefaultErrorPage.end(); ++it_three)
                    ofs << "    default error page " << it_three->first << " associated to path " << it_three->second << std::endl;
                std::list<std::string> getIndex = it_two->getIndex();
                for (std::list<std::string>::iterator it_three = getIndex.begin(); it_three != getIndex.end(); ++it_three)
                    ofs << "    index: " << *it_three << std::endl;
                ofs << "    autoindex: " << it_two->getAutoIndex() << std::endl;
                ofs << "    root: " << it_two->getRoot() << std::endl;
                std::map<std::string, std::string> getCgi = it_two->getCgi();
                for (std::map<std::string, std::string>::iterator it_three = getCgi.begin(); it_three != getCgi.end(); ++it_three)
                    ofs << "    cgi: extension " << it_three->first << " associated to path " << it_three->second << std::endl;
                if (it_two->getLocations().size() > 0)
                    location_recurs(it_two->getLocations(), 1);
            }
        }
    }

    void location_recurs(std::list<LocationScope> location_block, int level){
        std::ofstream ofs;
        ofs.open("./configurations/location_parsed.txt", std::ios_base::app);
        for (std::list<LocationScope>::iterator it_three = location_block.begin(); it_three != location_block.end(); ++it_three)
        {
            if (level == 1)
                ofs << "\n               ---| THE LOCATION SCOPE |---            \n" << std::endl;
            else
                ofs << "\n                    recursion level " << level << "\n" << std::endl;
            ofs << "        main_path: " << it_three->getMainPath() << std::endl;
            std::list<std::string> getTryFiles = it_three->getTryFiles();
            for (std::list<std::string>::iterator it_five = getTryFiles.begin(); it_five != getTryFiles.end(); ++it_five)
                ofs << "        try_files: " << *it_five << std::endl;
            std::list<std::string> getAllowMethod = it_three->getAllowMethod();
            for (std::list<std::string>::iterator it_five = getAllowMethod.begin(); it_five != getAllowMethod.end(); ++it_five)
                ofs << "        allow_method: " << *it_five << std::endl;
            ofs << "        client_body_buffer_size: " << it_three->getClientBodyBufferMax() << std::endl;
            std::map<std::string, std::string> getDefaultErrorPage = it_three->getDefaultErrorPage(); //to avoid dangling pointer warning
            for (std::map<std::string, std::string>::iterator it_five = getDefaultErrorPage.begin(); it_five != getDefaultErrorPage.end(); ++it_five)
                ofs << "        default error page " << it_five->first << " associated to path " << it_five->second << std::endl;
            std::list<std::string> getIndex = it_three->getIndex();
            for (std::list<std::string>::iterator it_five = getIndex.begin(); it_five != getIndex.end(); ++it_five)
                ofs << "        index: " << *it_five << std::endl;
            ofs << "        autoindex: " << it_three->getAutoIndex() << std::endl;
            ofs << "        root: " << it_three->getRoot() << std::endl;
            std::map<std::string, std::string> getCgi = it_three->getCgi();
            for (std::map<std::string, std::string>::iterator it_five = getCgi.begin(); it_five != getCgi.end(); ++it_five)
                ofs << "        cgi: extension " << it_five->first << " associated to path " << it_five->second << std::endl;
            if (it_three->getLocations().size() > 0)
                location_recurs(it_three->getLocations(), level + 1);
        }
    }
    //equal operation implementation
    void check_brackets();
    void extract_http_blocks();
    std::string chunking();

    private:

    std::string                 _file;
    std::list<HttpScope>       _http_blocks;
    class BracketsException : public std::exception {virtual const char* what() const throw(){return ("Close the brackets in the configuration file please");}};
};
