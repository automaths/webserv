#include "library.hpp"
// #include <stat.h>


	// if (access(path.data(), F_OK) == -1)
	// 	return (false);
	// if (stat(path.data(), buf) == -1)
	// 	return (false);

	// struct stat			buf;
	// else if (S_ISREG(buf.st_mode))
    	// if (S_ISDIR(buf.st_mode))

int main(int argc, char **argv)
{
    std::string uri = argv[1];
    std::string file;
    // std::string path_info;
    std::string copy = uri;
    std::string tmp;

    if (uri.size() > 0 && uri[0] == '/')
        uri.erase(0, 1);
    if (argc != 2)
        return 0;
    while (copy.find('/') != std::string::npos)
    {
        tmp = file;
        tmp += copy.substr(0, copy.find('/'));
        if (access(tmp.data(), F_OK) == -1)
        {
            std::cout << "cant access " << tmp << std::endl;
            break;
        }
        file += copy.substr(0, copy.find('/') + 1);
        copy.erase(0, copy.find('/') + 1);
    }
    if (file.size() != uri.size())
    {
        tmp = file;
        tmp += copy;
        if (access(tmp.data(), F_OK) != -1)
            file += copy;
    }
    


    
    std::cout << "The file is: " << file << std::endl;
    std::cout << "The path_info is: " << uri.substr(file.size(), uri.size() - file.size());

    return 0;
}


