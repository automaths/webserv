#pragma once

#include <algorithm>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
#include <exception>
#include <iostream>
#include <list>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <map>
#include <list>
#include <sstream>
#include <iostream>
#include <fstream>

//class Server;
//class Request;
#include "Request.hpp"
#include "Client.hpp"
#include "server.hpp"

#define GET "GET"
#define POST "POST"
#define DELETE "DELETE"
