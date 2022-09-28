#pragma once

#include <map>
#include <list>
#include "Client.hpp"
#include <utility>
#include <ctime>
#define PORT 8080

// <link rel='icon' href='data:,'> prevent automatic favicon request
class Server {
    public:

    Server(){}
    ~Server()
	{
		close(this->_epoll_fd);
		for (std::map<int, int>::iterator st = this->_listen_sockets.begin(); st!= this->_listen_sockets.end(); st++)
		{
			close((*st).first);
		}
		for (std::map<int, Client>::iterator st = this->_client_sockets.begin(); st!= this->_client_sockets.end(); st++)
		{
			close((*st).first);
		}
		this->_listen_sockets.clear();
		this->_client_sockets.clear();
	}

//    int getServerFd(){return _server_fd;}
    struct sockaddr_in getAddr(){return _address;}

    void initing(){
		int	_server_fd;
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
		this->_listen_sockets.insert(std::pair<int, int>(_server_fd, PORT));
		if ((this->_epoll_fd = epoll_create(1)) == -1)
			throw EpollCreateException();
    }

    void execute(){
		std::string	_test_cat_start("HTTP/1.1 200 OK\nDate: Mon, 27 Jul 2022 12:28:53 GMT\nServer: Apache/2.2.14 (Win32)\nLast-Modified: Wed, 22 Jul 2022 19:15:56 GMT\nContent-Length: 88\nContent-Type: text/html\nConnection: Closed\n\n<html>\n<head>\n<meta http-equiv='Refresh' content='0; URL=https://http.cat/");
		std::string _test_cat_end("' />\n</head>\n</html>");

		std::map<int, int>::iterator	serv;
		struct epoll_event	ev, event[10];
        int result;
        int addrlen = sizeof(_address);


		for (std::map<int, int>::const_iterator st = this->_listen_sockets.begin(); st!= this->_listen_sockets.end(); st++)
		{
			ev.events = EPOLLIN;
			ev.data.fd = (*st).first;
			if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, (*st).first, &ev) == -1)
				throw EpollCreateException();
		}
        while (true)
        {
			result = epoll_wait(this->_epoll_fd, event, 10, 1000);
			if (result == -1)
				throw EpollCreateException();
			for (int i = 0; i < result; i++)
			{
				if ((serv = this->_listen_sockets.find(event[i].data.fd)) != this->_listen_sockets.end())
				{
					Client	tmp(this->_listen_sockets[event[i].data.fd]);
					ev.events = EPOLLIN;
					ev.data.fd = accept((*serv).first, (struct sockaddr *)&_address, (socklen_t*)&addrlen);
					if (ev.data.fd < 0)
                        throw AcceptException();
					if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, ev.data.fd, &ev) == -1)
						throw EpollCreateException();
					this->_client_sockets.insert(std::pair<int, Client>(ev.data.fd, tmp)); 
				}
				else if (event[i].events == EPOLLIN)
				{
					char	buffer[10001];
					ssize_t	recvret = 0;

                    recvret = recv(event[i].data.fd, static_cast<void *>(buffer), 10000, MSG_DONTWAIT);
					if (recvret == -1)
						std::cerr << "Error while receiving data" << std::endl;
					else if (recvret == 0)
					{
						epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, event[i].data.fd, &event[i]);
						close(event[i].data.fd);
						this->_client_sockets.erase(event[i].data.fd);
					}
					else if (recvret < 10000)
					{
						buffer[recvret] = '\0';
						std:: cout << buffer << "size: " << recvret << std::endl;
						std::cout << "fd used: " << event[i].data.fd << std::endl;
						std::cout << "Port Number: " << this->_client_sockets[event[i].data.fd].getPortNumber() << std::endl;
						result = this->_client_sockets[event[i].data.fd].addToRequest(std::string(buffer));
						if (result == -1)
						{
							std::cerr << "BAD REQUEST" << std::endl;
							epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, event[i].data.fd, NULL);
							close(event[i].data.fd);
							this->_client_sockets.erase(event[i].data.fd);
						}
						else if (result == 1)
						{
							event[i].events = EPOLLOUT;
							if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_MOD, event[i].data.fd, &event[i]) == -1)
								throw EpollCreateException();
						}
					}
					else
					{
						std::cout << "Buffer incomplete, need class" << std::endl;
					}
				}
				else if (event[i].events == EPOLLOUT)
				{
					std::cerr << "Sending" << std::endl;
					ssize_t	sendret = 0;
					std::string sender = _test_cat_start + std::string("200") + _test_cat_end;
					sendret = send(event[i].data.fd, sender.data(), sender.length(), MSG_NOSIGNAL | MSG_DONTWAIT);
					if (sendret == -1)
						std::cerr << "Error while sending" << std::endl;
					else if (sendret >= (long)sender.length())
					{
						this->_client_sockets[event[i].data.fd].resetRequest();
						if (this->_client_sockets[event[i].data.fd].getKeepAlive() != -1)
						{
							event[i].events = EPOLLIN;
							if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_MOD, event[i].data.fd, &event[i]) == -1)
								throw EpollCreateException();
						}
						else
						{	
							if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, event[i].data.fd, &event[i]) == -1)
								throw EpollCreateException();
							close(event[i].data.fd);
							this->_client_sockets.erase(event[i].data.fd);
						}
					}
					else
						std::cerr << "More data to send" << std::endl;

				}
			}
			this->closeTimedoutConnections();
			std::cout << "The number of clients is: " << this->_client_sockets.size() << std::endl;
        }
    }

	void	closeTimedoutConnections(void)
	{
		std::time_t	current = std::time(NULL);
		std::map<int, Client>::iterator st = this->_client_sockets.begin();
		std::map<int, Client>::iterator tmp;

		while (st!= this->_client_sockets.end())
		{
			tmp = st;
			tmp++;
			if (std::difftime(current, (*st).second.getLastConnection()) > (*st).second.getKeepAlive())
			{
				epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, (*st).first, NULL);
				close((*st).first);
				this->_client_sockets.erase(st);
			}
			st = tmp;
		}
	}

    private:
    
	int						_epoll_fd;			//fd for epoll queue
	std::map<int, int>		_listen_sockets;	//Listening sockets: <fd, port>
	std::map<int, Client>	_client_sockets;	//Accepted connections sockets: <fd, Client>
    struct sockaddr_in		_address;			// Address to bind to a socket

    class SocketCreationException : public std::exception {virtual const char* what() const throw(){return ("An error occured during socket creation");}};
    class BindException : public std::exception {virtual const char* what() const throw(){return ("An error occured during bind");}};
    class ListenException : public std::exception {virtual const char* what() const throw(){return ("An error occured during listen");}};
    class SelectException : public std::exception {virtual const char* what() const throw(){return ("An error occured during select");}};
    class EpollCreateException : public std::exception {virtual const char* what() const throw(){return ("An error occured while creating an epoll instance");}};
    class AcceptException : public std::exception {virtual const char* what() const throw(){return ("An error occured during accept");}};
    class ReadException : public std::exception {virtual const char* what() const throw(){return ("An error occured during read");}};
    class SendException : public std::exception {virtual const char* what() const throw(){return ("An error occured during send");}};

};
