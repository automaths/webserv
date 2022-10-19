#pragma once

# include <map>
# include <string>


class MimeTypes {

    public:

    MimeTypes();
    ~MimeTypes();
    std::string convert(std::string extension);

    private:
    
    std::map<std::string, std::string>  _mimetypes;
};
