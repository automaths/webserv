#pragma once

#include <map>
#include <list>
#include "Client.hpp"
#include <utility>
#include <ctime>
#include <cstring>
#define PORT 8080

class Server {
    public:

    Server():_epoll_fd(-1){}
    ~Server()
	{
		if (this->_epoll_fd != -1)
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

//	bool	acceptIncomingConnection(struct epoll_event event[i])

    void execute(){
		std::map<int, int>::iterator	serv;
		struct epoll_event	ev, event[10];
        int result;
        int addrlen = sizeof(_address);

//		std::memset(event, '\0', (sizeof(struct epoll_event) * 10));
		std::memset(&ev, '\0', sizeof(struct epoll_event));

		for (std::map<int, int>::iterator st = this->_listen_sockets.begin(); st!= this->_listen_sockets.end(); st++)
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
//					acceptIncomingConnection(event[i]);
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
					else
					{
						buffer[recvret] = '\0';
						result = this->_client_sockets[event[i].data.fd].addToRequest(std::string(buffer));
						std::cerr << "Result parsing: " << result << std::endl;
						if (result == 200)
						{
							this->_client_sockets[event[i].data.fd].getResponse() = Response(this->_client_sockets[event[i].data.fd].getRequest());
							event[i].events = EPOLLOUT;
							if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_MOD, event[i].data.fd, &event[i]) == -1)
								throw EpollCreateException();
						}
						else if (result)
						{
							this->_client_sockets[event[i].data.fd].getResponse() = Response(this->_client_sockets[event[i].data.fd].getRequest(), 400);
							event[i].events = EPOLLOUT;
							if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_MOD, event[i].data.fd, &event[i]) == -1)
								throw EpollCreateException();
						}
					}
				}
				else if (event[i].events == EPOLLOUT)
				{
					ssize_t	sendret = 0;
					std::cerr << "Sending" << std::endl;
					if (!this->_client_sockets[event[i].data.fd].getResponse().headerIsSent())
					{
						sendret = send(event[i].data.fd, this->_client_sockets[event[i].data.fd].getResponse().getHeader().data(), this->_client_sockets[event[i].data.fd].getResponse().getHeaderSize(), MSG_NOSIGNAL | MSG_DONTWAIT);
						if (sendret == -1)
						{
							std::cerr << "Error while sending" << std::endl;
							if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, event[i].data.fd, &event[i]) == -1)
								throw EpollCreateException();
							close(event[i].data.fd);
							this->_client_sockets.erase(event[i].data.fd);
							continue ;
						}
						if (this->_client_sockets[event[i].data.fd].getResponse().headerBytesSent(sendret))
						{
							sendret = send(event[i].data.fd, this->_client_sockets[event[i].data.fd].getResponse().getBody().data(), this->_client_sockets[event[i].data.fd].getResponse().getBodySize(), MSG_NOSIGNAL | MSG_DONTWAIT);
							if (sendret == -1)
							{	
								std::cerr << "Error while sending" << std::endl;
								if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, event[i].data.fd, &event[i]) == -1)
									throw EpollCreateException();
								close(event[i].data.fd);
								this->_client_sockets.erase(event[i].data.fd);
								continue ;
							}
							if (this->_client_sockets[event[i].data.fd].getResponse().bodyBytesSent(sendret))
							{
								if (this->_client_sockets[event[i].data.fd].getResponse().getClose())
								{
									if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, event[i].data.fd, &event[i]) == -1)
										throw EpollCreateException();
									close(event[i].data.fd);
									this->_client_sockets.erase(event[i].data.fd);
								}
								else
								{
									this->_client_sockets[event[i].data.fd].resetRequest();
									this->_client_sockets[event[i].data.fd].resetResponse();
									event[i].events = EPOLLIN;
									if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_MOD, event[i].data.fd, &event[i]) == -1)
										throw EpollCreateException();
								}
							}
						}
					}
					else
					{
							sendret = send(event[i].data.fd, this->_client_sockets[event[i].data.fd].getResponse().getBody().data(), this->_client_sockets[event[i].data.fd].getResponse().getBodySize(), MSG_NOSIGNAL | MSG_DONTWAIT);
							if (sendret == -1)
							{	
								std::cerr << "Error while sending" << std::endl;
								if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, event[i].data.fd, &event[i]) == -1)
									throw EpollCreateException();
								close(event[i].data.fd);
								this->_client_sockets.erase(event[i].data.fd);
								continue ;
							}
							if (this->_client_sockets[event[i].data.fd].getResponse().bodyBytesSent(sendret))
							{
								if (this->_client_sockets[event[i].data.fd].getResponse().getClose())
								{
									if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, event[i].data.fd, &event[i]) == -1)
										throw EpollCreateException();
									close(event[i].data.fd);
									this->_client_sockets.erase(event[i].data.fd);
								}
								else
								{
									this->_client_sockets[event[i].data.fd].resetRequest();
									this->_client_sockets[event[i].data.fd].resetResponse();
									event[i].events = EPOLLIN;
									if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_MOD, event[i].data.fd, &event[i]) == -1)
										throw EpollCreateException();
								}
							}

					}
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
