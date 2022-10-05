#include "../library.hpp"
#include "cgi.hpp"
#include <unistd.h>

int main()
{
    std::vector<std::string> env;
    std::string name = "/ex.php";    
    char buffer[500];
    std::string pwd(getcwd(buffer, 500));
    pwd.append(name);
    std::string script_name = "SCRIPT_FILENAME=";
    script_name.append(pwd);

    env.push_back(script_name);
    env.push_back("SERVER_PROTOCOL=HTTP/1.1");
    env.push_back("SERVER_PORT=80");
    env.push_back("REQUEST_METHOD=GET");
    env.push_back("PATH_INFO=/usr/lib/cgi-bin/php");
    env.push_back("REDIRECT_STATUS=1");

    int fd = open("ex.php", O_RDONLY);
    if (fd == -1)
        return (printf("open error\n"), 0);

    Cgi test(fd, env);

    std::pair<int, int> result;
    result = test.getResult();

    // if (result.second == 0)
    //     std::cout << "success" << std::endl;
    // if (result.second == 1  || result.second == 256)
    //     std::cout << "error occured" << std::endl;
    // if (result.second == 126)
    //     std::cout << "cannot execute" << std::endl;
    // if (result.second == 127)
    //     std::cout << "not found" << std::endl;

    char buff[1048576];
    int size = read(result.first, &buff, 1048576);
    //the content-length for the header is the size
    if (size == -1)
        return (printf("read error\n"), 0);
    write(1, &buff, size);
    close(result.first);

    return 0;
}
