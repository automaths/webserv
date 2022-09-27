#pragma once


#define PORT 8080

// <link rel='icon' href='data:,'> prevent automatic favicon request
class Server {
    public:

    Server(){}
    ~Server(){close(this->_epoll_fd);}

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
		if ((this->_epoll_fd = epoll_create(1)) == -1)
			throw EpollCreateException();
    }

    void execute(){
		std::string	_test_cat_start("HTTP/1.1 200 OK\nDate: Mon, 27 Jul 2022 12:28:53 GMT\nServer: Apache/2.2.14 (Win32)\nLast-Modified: Wed, 22 Jul 2022 19:15:56 GMT\nContent-Length: 88\nContent-Type: text/html\nConnection: Closed\n\n<html>\n<head>\n<meta http-equiv='Refresh' content='0; URL=https://http.cat/");
		std::string _test_cat_end("' />\n</head>\n</html>");

		struct epoll_event	ev, event[10];
        int result;
        int addrlen = sizeof(_address);

		ev.events = EPOLLIN;
		ev.data.fd = this->_server_fd;
		if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, this->_server_fd, &ev) == -1)
			throw EpollCreateException();
        while (1)
        {
			result = epoll_wait(this->_epoll_fd, event, 10, -1);
			if (result == -1)
				throw EpollCreateException();
			for (int i = 0; i < result; i++)
			{
				if (event[i].data.fd == this->_server_fd)
				{
					ev.events = EPOLLIN;
					ev.data.fd = accept(_server_fd, (struct sockaddr *)&_address, (socklen_t*)&addrlen);
					if (ev.data.fd < 0)
                        throw AcceptException();
					if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, ev.data.fd, &ev) == -1)
						throw EpollCreateException();
					_client_set.push_back(ev.data.fd);
				}
				else if (event[i].events == EPOLLIN)
				{
					char	buffer[10000];
					ssize_t	recvret = 0;

                    recvret = recv(event[i].data.fd, static_cast<void *>(buffer), 10000, MSG_DONTWAIT);
					if (recvret == -1)
						std::cerr << "Error while receiving data" << std::endl;
					else if (recvret < 10000)
					{
						std:: cout << buffer << std::endl;
						
						event[i].events = EPOLLOUT;
						if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_MOD, event[i].data.fd, &event[i]) == -1)
							throw EpollCreateException();
					}
					else
					{
						std::cout << "Buffer incomplete, need class" << std::endl;
					}
				}
				else if (event[i].events == EPOLLOUT)
				{
					ssize_t	sendret = 0;
					std::string sender = _test_cat_start + std::string("200") + _test_cat_end;
					sendret = send(event[i].data.fd, sender.data(), sender.length(), MSG_NOSIGNAL | MSG_DONTWAIT);
					if (sendret == -1)
						std::cerr << "Error while sending" << std::endl;
					else if (sendret >= (long)sender.length())
					{
						if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, event[i].data.fd, &event[i]) == -1)
							throw EpollCreateException();
						close(event[i].data.fd);
						_client_set.erase(std::find(_client_set.begin(), _client_set.end(), event[i].data.fd)); 
					}
					else
						std::cerr << "More data to send" << std::endl;

				}
			}
            printf("The number of clients is %zu\n", _client_set.size());
        }
    }

    private:
    
    int _server_fd;
	int	_epoll_fd;
    struct sockaddr_in _address;                  /* Set of socket descriptors for select() */      /* Timeout for select() */
    std::list<int> _client_set;

    class SocketCreationException : public std::exception {virtual const char* what() const throw(){return ("An error occured during socket creation");}};
    class BindException : public std::exception {virtual const char* what() const throw(){return ("An error occured during bind");}};
    class ListenException : public std::exception {virtual const char* what() const throw(){return ("An error occured during listen");}};
    class SelectException : public std::exception {virtual const char* what() const throw(){return ("An error occured during select");}};
    class EpollCreateException : public std::exception {virtual const char* what() const throw(){return ("An error occured while creating an epoll instance");}};
    class AcceptException : public std::exception {virtual const char* what() const throw(){return ("An error occured during accept");}};
    class ReadException : public std::exception {virtual const char* what() const throw(){return ("An error occured during read");}};
    class SendException : public std::exception {virtual const char* what() const throw(){return ("An error occured during send");}};

};
