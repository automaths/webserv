#include "library.hpp"
#include <stat.h>


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

    if (argc != 2)
        return 0;
    while (copy.find('/') != std::string::npos)
    {
        tmp = file;
        tmp += copy.substr(0, copy.find('/') + 1);
        if (access(tmp.data(), F_OK) == -1)
            break;
        file += copy.substr(0, copy.find('/') + 1);
        copy.erase(0, copy.find('/') + 1);
    }
    if (file.size() != uri.size())
    {
        if (access(file.data(), F_OK) != -1)
            file += uri;
    }


    if (copy.size() > 0)
        file += tmp;





    return 0;
}


