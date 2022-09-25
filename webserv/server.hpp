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
    
    int _server_fd;
    struct sockaddr_in _address;
    // char *_test_cat_start = "HTTP/1.1 200 OK\nDate: Mon, 27 Jul 2009 12:28:53 GMT\nServer: Apache/2.2.14 (Win32)\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT\nContent-Length: 88\nContent-Type: text/html\nConnection: Closed\n\n<html>\n<head>\n<meta http-equiv='Refresh' content='0; URL=https://http.cat/";
    // char *_test_cat_end = "' />\n</head>\n</html>";
    int _new_socket;
    int _addrlen = sizeof(_address);
    fd_set _sockSet;                  /* Set of socket descriptors for select() */
    struct timeval _timeout;       /* Timeout for select() */
    int    _i;
    int    _result;
    int _number_clients;
    int _client[1000];

    public:

    class SocketCreationException : public std::exception {virtual const char* what() const throw(){return ("An error occured during socket creation");}};
    class BindException : public std::exception {virtual const char* what() const throw(){return ("An error occured during bind");}};
    class ListenException : public std::exception {virtual const char* what() const throw(){return ("An error occured during listen");}};
    class SelectException : public std::exception {virtual const char* what() const throw(){return ("An error occured during select");}};
    class AcceptException : public std::exception {virtual const char* what() const throw(){return ("An error occured during accept");}};
    class ReadException : public std::exception {virtual const char* what() const throw(){return ("An error occured during accept");}};
    class WriteException : public std::exception {virtual const char* what() const throw(){return ("An error occured during accept");}};

    int getServerFd(){return _server_fd;}
    struct sockaddr_in getAddr(){return _address;}

    void initing(){
        // creating the socket
        if ((_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
            throw SocketCreationException();
        // configuring the socket model
        _address.sin_family = AF_INET;
        _address.sin_addr.s_addr = INADDR_ANY;
        _address.sin_port = htons( PORT );
        memset(_address.sin_zero, '\0', sizeof _address.sin_zero);
        // adding the configurations on the created socket 
        if (bind(_server_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0)
            throw BindException();
        // opening to right port to receive requests
        if (listen(_server_fd, 10) < 0)
            throw ListenException();
        _number_clients = 0;
    }

    void execute(){
        char *_test_cat_start = (char *)"HTTP/1.1 200 OK\nDate: Mon, 27 Jul 2009 12:28:53 GMT\nServer: Apache/2.2.14 (Win32)\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT\nContent-Length: 88\nContent-Type: text/html\nConnection: Closed\n\n<html>\n<head>\n<meta http-equiv='Refresh' content='0; URL=https://http.cat/";
        char *_test_cat_end = (char *)"' />\n</head>\n</html>";
        int size_read;
        int k;
        while (1)
        {
            // the fd_set needs to be configured for each call to select
            FD_ZERO(&_sockSet); // initing to null the fd_set
            FD_SET(_server_fd, &_sockSet); // put server socket to fd_set
            for (int i = 0; i < _number_clients; i++) 
                FD_SET(_client[i], &_sockSet); // put each client socket to the fd_set
            _timeout.tv_sec = 1; _timeout.tv_usec = 0; // set timeout

            _result = select(10, &_sockSet, NULL, NULL, &_timeout); // max 10 requests, only checking reads

            if (_result == -1)
                throw SelectException(); 
            else if(_result > 0) // number of sockets ready
            {
                if (FD_ISSET(_server_fd, &_sockSet)) // test the server socket to see if there is incoming connection
                {
                    if ((_new_socket = accept(_server_fd, (struct sockaddr *)&_address, (socklen_t*)&_addrlen)) < 0)
                        throw AcceptException();
                    else
                        _client[_number_clients++] = _new_socket;
                }
                // test all client sockets to see which ones are ready
                for (int i = 0; i < _number_clients; i++)
                    if (FD_ISSET(_client[i], &_sockSet) && _client[i] != -1)
                    {
                        char buffer[10000];
                        if (read( _client[i] , buffer, 10000) == -1)
                            throw ReadException();
                        printf("%s\n",buffer ); // request to parse and give a response to 
                        k = 0;
                        if (write(_client[i] , _test_cat_start , strlen(_test_cat_start)) == -1) // the request response
                            throw WriteException();
                        while (buffer[k + 5] >= '0' && buffer[k + 5] <= '9')
                            if (write(_client[i], &buffer[k++ + 5], 1))
                                throw WriteException();
                        if (write(_client[i] , _test_cat_end , strlen(_test_cat_end)))
                            throw WriteException();
                        close (_client[i]);
                        _client[i] = -1;
                    }

            }
            printf("The number of clients is %d\n", _number_clients);
        }
    }

};
