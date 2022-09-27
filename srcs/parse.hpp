#pragma once

#include "library.hpp"

class Request{

    public:

    Request(){};
    ~Request(){};

    int ParseChunk(std::string chunk){
        (void)chunk;
        return 0;
    }

    private:

    std::string type;
    std::map<std::string, std::list<std::string> > headers;
    std::string body;
};