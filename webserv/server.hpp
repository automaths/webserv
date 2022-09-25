#pragma once

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
#include <exception>

#define PORT 8080

class Server {
    private:
    
    int server_fd;
    struct sockaddr_in address;


    public:

    class SocketCreationError : public std::exception {virtual const char* what() const throw(){return ("An error occured during socket creation");}};
    class BindError : public std::exception {virtual const char* what() const throw(){return ("An error occured during bind");}};
    class ListenError : public std::exception {virtual const char* what() const throw(){return ("An error occured during listen");}};
    class SelectError : public std::exception {virtual const char* what() const throw(){return ("An error occured during select");}};
    class AcceptError : public std::exception {virtual const char* what() const throw(){return ("An error occured during accept");}};


    void initing(){

    }


};
