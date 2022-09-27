#pragma once

#include "library.hpp"

class Request{

    public:

    Request(){};
    ~Request(){};

    int ParseChunk(std::string chunk){

    }

    private:

    std::string type;
    std::map<std::string, std::list> headers;
    std::string body;
};