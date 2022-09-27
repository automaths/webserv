#pragma once

#include "library.hpp"

class Request{

    public:

    Request(){};
    ~Request(){};

    ParseChunk(std::string chunk){

    }

    private:

    std::string type;
    std::map<std::string, std::list> headers;
    std::string body;
};