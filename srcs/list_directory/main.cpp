#include "../library.hpp"
#include "ListDirectory.hpp"

int main()
{
    ListDirectory coucou("/mnt/nfs/homes/nsartral/Github/webserv/srcs/list_directory", "data/list_directory");
    coucou.listing();

    return 0;
}


