#pragma once

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

#define PORT 8080

class Server {
    public:

    Server(){}
    ~Server(){}

    int getServerFd(){return _server_fd;}
    struct sockaddr_in getAddr(){return _address;}

    void initing(){
        if ((_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) // creating the socket
            throw SocketCreationException();
        _address.sin_family = AF_INET; // socket configuration
        _address.sin_addr.s_addr = INADDR_ANY;
        _address.sin_port = htons( PORT );
        memset(_address.sin_zero, '\0', sizeof _address.sin_zero); // applying configurations on the created socket
        if (bind(_server_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0)
            throw BindException();
        if (listen(_server_fd, 10) < 0) // opening the socket to the port
            throw ListenException();
    }

    void execute(){
        char *_test_cat_start = (char *)"HTTP/1.1 200 OK\nDate: Mon, 27 Jul 2022 12:28:53 GMT\nServer: Apache/2.2.14 (Win32)\nLast-Modified: Wed, 22 Jul 2022 19:15:56 GMT\nContent-Length: 88\nContent-Type: text/html\nConnection: Closed\n\n<html>\n<head>\n<meta http-equiv='Refresh' content='0; URL=https://http.cat/";
        char *_test_cat_end = (char *)"' />\n</head>\n</html>";
        int tmp;
        int result;
        int addrlen = sizeof(_address);
        struct timeval timeout;
        fd_set _sockSet; // Set of socket descriptors for select

        while (1)
        {
            // the fd_set needs to be configured for each call to select
            FD_ZERO(&_sockSet); // initing to null the fd_set
            FD_SET(_server_fd, &_sockSet); // put server socket to fd_set
            for (std::list<int>::iterator it = _client_set.begin(); it != _client_set.end(); ++it)
                FD_SET(*it, &_sockSet); // put each client socket to the fd_set
            timeout.tv_sec = 1; timeout.tv_usec = 0; // set timeout

            result = select(10, &_sockSet, NULL, NULL, &timeout); // max 10 requests, only checking reads

            if (result == -1)
                throw SelectException(); 
            else if(result > 0) // number of sockets ready
            {
                if (FD_ISSET(_server_fd, &_sockSet)) // test the server socket to see if there is incoming connection
                {
                    if ((tmp = accept(_server_fd, (struct sockaddr *)&_address, (socklen_t*)&addrlen)) < 0)
                        throw AcceptException();
                    else
                        _client_set.push_back(tmp);
                }
                // test all client sockets to see which ones are ready
                for (std::list<int>::iterator it = _client_set.begin(); it != _client_set.end(); ++it)
                    if (FD_ISSET(*it, &_sockSet))
                    {
                        char buffer[10000];
                        if (read(*it, buffer, 10000) == -1)
                            throw ReadException();
                        printf("%s\n",buffer ); // request to parse and give a response to 
                        tmp = 0;
                        if (write(*it, _test_cat_start , strlen(_test_cat_start)) == -1) // the request response
                            throw WriteException();
                        while (buffer[tmp + 5] >= '0' && buffer[tmp + 5] <= '9')
                            if (write(*it, &buffer[tmp++ + 5], 1) == -1)
                                throw WriteException();
                        if (write(*it , _test_cat_end , strlen(_test_cat_end)) == -1)
                            throw WriteException();
                        close (*it);
                    }
            }
            for (std::list<int>::iterator it = _client_set.begin(); it != _client_set.end(); ++it)
            {
                if (fcntl(*it, F_GETFL) < 0 && errno == EBADF)
                {
                    _client_set.erase(it);
                    it = _client_set.begin();
                }
            }
            printf("The number of clients is %zu\n", _client_set.size());
        }
    }

    private:
    
    int _server_fd;
    struct sockaddr_in _address;                  /* Set of socket descriptors for select() */      /* Timeout for select() */
    std::list<int> _client_set;

    class SocketCreationException : public std::exception {virtual const char* what() const throw(){return ("An error occured during socket creation");}};
    class BindException : public std::exception {virtual const char* what() const throw(){return ("An error occured during bind");}};
    class ListenException : public std::exception {virtual const char* what() const throw(){return ("An error occured during listen");}};
    class SelectException : public std::exception {virtual const char* what() const throw(){return ("An error occured during select");}};
    class AcceptException : public std::exception {virtual const char* what() const throw(){return ("An error occured during accept");}};
    class ReadException : public std::exception {virtual const char* what() const throw(){return ("An error occured during accept");}};
    class WriteException : public std::exception {virtual const char* what() const throw(){return ("An error occured during accept");}};

};
