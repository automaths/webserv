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
    int _size_read;
    int _iNumClients;
    int _clients[1000];

    public:

    class SocketCreationException : public std::exception {virtual const char* what() const throw(){return ("An error occured during socket creation");}};
    class BindException : public std::exception {virtual const char* what() const throw(){return ("An error occured during bind");}};
    class ListenException : public std::exception {virtual const char* what() const throw(){return ("An error occured during listen");}};
    class SelectException : public std::exception {virtual const char* what() const throw(){return ("An error occured during select");}};
    class AcceptException : public std::exception {virtual const char* what() const throw(){return ("An error occured during accept");}};

    int getServerFd(){return _server_fd;}
    struct sockaddr_in getAddr(){return _address;}

    void initing(){
        if ((_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
            throw SocketCreationException();
        _address.sin_family = AF_INET;
        _address.sin_addr.s_addr = INADDR_ANY;
        _address.sin_port = htons( PORT );
        memset(_address.sin_zero, '\0', sizeof _address.sin_zero);
        if (bind(_server_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0)
            throw BindException();
        if (listen(_server_fd, 10) < 0)
            throw ListenException();

        _iNumClients = 0;
    }

    void execute(){
        char *_test_cat_start = (char *)"HTTP/1.1 200 OK\nDate: Mon, 27 Jul 2009 12:28:53 GMT\nServer: Apache/2.2.14 (Win32)\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT\nContent-Length: 88\nContent-Type: text/html\nConnection: Closed\n\n<html>\n<head>\n<meta http-equiv='Refresh' content='0; URL=https://http.cat/";
        char *_test_cat_end = (char *)"' />\n</head>\n</html>";
        while (1)
        {
            /* Zero socket descriptor vector and set for server sockets */
            /* This must be reset every time select() is called */
            FD_ZERO(&_sockSet);
            FD_SET(_server_fd, &_sockSet);
            for (int i=0; i < _iNumClients; i++)
                FD_SET(_clients[i], &_sockSet);

            /* Timeout specification */
            /* This must be reset every time select() is called */
            _timeout.tv_sec = 1;       /* _timeout (secs.) */
            _timeout.tv_usec = 0;            /* 0 microseconds */

            _result = select(10, &_sockSet, NULL, NULL, &_timeout);

            if (_result == -1)
                printf("an error occured during select");
            else if(_result > 0) /* ie, if a socket is ready */
            {
                printf("Found at least one ready socket\n");
                // test this specific socket to see if it was one of the ones that was set
                if (FD_ISSET(_server_fd, &_sockSet))
                {
                    if ((_new_socket = accept(_server_fd, (struct sockaddr *)&_address, (socklen_t*)&_addrlen))<0)
                        throw AcceptException();
                    else
                    {
                        _clients[_iNumClients] = _new_socket;
                        ++_iNumClients;
                    }
                }
                
                /* Now test the client sockets */
                for (int i=0; i < _iNumClients; i++)
                    if (FD_ISSET(_clients[i], &_sockSet) && _clients[i] != -1)
                    {
                        /* do whatever it takes to read the socket and handle the client */
                        /* Please note that this will involve reassociating the socket   */
                        /*     with the client record                                    */
                        char buffer[30000] = {0};
                        _size_read = read( _clients[i] , buffer, 30000);
                        printf("%s\n",buffer );

                        int k = 0;

                        write(_clients[i] , _test_cat_start , strlen(_test_cat_start));
                        while (buffer[k + 5] >= '0' && buffer[k + 5] <= '9')
                            write(_clients[i], &buffer[k++ + 5], 1);
                        write(_clients[i] , _test_cat_end , strlen(_test_cat_end));

                        close (_clients[i]);
                        _clients[i] = -1;
                    }

            }

            /* else result == 0, no socket is ready to be read, */
            /*    so ignore them and move on.                    */

            printf("The number of clients is %d\n", _iNumClients);
        }
    }

};
