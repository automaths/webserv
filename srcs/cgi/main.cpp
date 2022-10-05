#include "../library.hpp"
#include "cgi.hpp"

int main()
{
    std::vector<std::string> env;

    env.push_back("SERVER_PROTOCOL=HTTP/1.1");
    env.push_back("SERVER_PORT=80");
    env.push_back("REQUEST_METHOD=GET");
    env.push_back("PATH_INFO=usr/lib/cgi-bin/php");

    int fd = open("ex.php", O_RDONLY);
    if (fd == -1)
        return (printf("open error"));
    Cgi test(fd, env);

    return 0;
}
