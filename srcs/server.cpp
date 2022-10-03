/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdetune <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/28 13:45:50 by bdetune           #+#    #+#             */
/*   Updated: 2022/10/03 13:04:21 by bdetune          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

Server::Server(void): _epoll_fd(-1)
{
	return ;
}

Server::~Server(void)
{
	if (this->_epoll_fd != -1)
		close(this->_epoll_fd);
	for (std::map<int, Client>::iterator st = this->_client_sockets.begin(); st!= this->_client_sockets.end(); st++)
	{
		shutdown((*st).first, SHUT_RDWR);
		close((*st).first);
	}
	for (std::map<int, int>::iterator st = this->_listen_sockets.begin(); st!= this->_listen_sockets.end(); st++)
	{
		close((*st).first);
	}
	this->_client_sockets.clear();
	this->_listen_sockets.clear();
}

struct sockaddr_in Server::getAddr(void)
{
	return (this->_address);
}

void Server::initing(void)
{
	int	_server_fd;
	int	enable = 1;
        if ((_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) // creating the socket
            throw SocketCreationException();
        _address.sin_family = AF_INET; // socket configuration
        _address.sin_addr.s_addr = INADDR_ANY;
        _address.sin_port = htons( PORT );
        memset(_address.sin_zero, '\0', sizeof _address.sin_zero); // applying configurations on the created socket
		if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)))
            throw BindException();
        if (bind(_server_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0)
            throw BindException();
        if (listen(_server_fd, 10) < 0) // opening the socket to the port
            throw ListenException();
		this->_listen_sockets.insert(std::pair<int, int>(_server_fd, PORT));
		if ((this->_epoll_fd = epoll_create(1)) == -1)
			throw EpollCreateException();
}

bool	Server::epollSockets(void)
{
	struct epoll_event	ev;

	std::memset(&ev, '\0', sizeof(struct epoll_event));
	for (std::map<int, int>::iterator st = this->_listen_sockets.begin(); st!= this->_listen_sockets.end(); st++)
	{
		ev.events = EPOLLIN;
		ev.data.fd = (*st).first;
		if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, (*st).first, &ev) == -1)
			return (false) ;
	}
	return (true);
}

bool	Server::acceptIncomingConnection(struct epoll_event & event)
{
	Client				tmp(this->_listen_sockets[event.data.fd]);
	struct epoll_event	ev;
    int					addrlen	=sizeof(_address);

	std::memset(&ev, '\0', sizeof(struct epoll_event));
	ev.events = EPOLLIN;
	ev.data.fd = accept(event.data.fd, (struct sockaddr *)&_address, (socklen_t*)&addrlen);
	if (ev.data.fd < 0)
		return (false);
	this->_client_sockets.insert(std::pair<int, Client>(ev.data.fd, tmp));
	if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, ev.data.fd, &ev) == -1)
		return (close(ev.data.fd), false);
	return (true);
}

void	Server::readRequest(struct epoll_event & event)
{
	int		result;
	ssize_t	recvret = 0;

	recvret = recv(event.data.fd, static_cast<void *>(_rdBuffer), 1048576, MSG_DONTWAIT);
	if (recvret == -1)
	{
		std::cerr << "Error while trying to read socket" << std::endl;
	}
	else if (recvret == 0)
	{
		std::cerr << "Client closed connection while receiving data" << std::endl;
		epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
		close(event.data.fd);
		this->_client_sockets.erase(event.data.fd);
	}
	else
	{
		_rdBuffer[recvret] = '\0';
		try
		{
			result = this->_client_sockets[event.data.fd].addToRequest(std::string(_rdBuffer));
		}
		catch (std::exception const & e)
		{
			std::cerr << "Parsing error: " << e.what() << std::endl;
			return ;
		}
		std::cerr << "Result parsing: " << result << std::endl;
		if (result == 200)
		{
			this->_client_sockets[event.data.fd].getResponse() = Response(this->_client_sockets[event.data.fd].getRequest());
			event.events = EPOLLOUT;
			if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_MOD, event.data.fd, &event) == -1)
			{
				epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
				close(event.data.fd);
				this->_client_sockets.erase(event.data.fd);
				std::cerr << "Error Trying to switch socket from reading to writing" << std::endl;
			}
		}
		else if (result)
		{
			this->_client_sockets[event.data.fd].getResponse() = Response(this->_client_sockets[event.data.fd].getRequest(), 400);
			event.events = EPOLLOUT;
			if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_MOD, event.data.fd, &event) == -1)
			{
				epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
				close(event.data.fd);
				this->_client_sockets.erase(event.data.fd);
				std::cerr << "Error Trying to switch socket from reading to writing" << std::endl;
			}
		}
	}
}

void Server::execute(){
		struct epoll_event	ev, event[10];
        int ready;

		if (!this->epollSockets())
			throw EpollCreateException();
		std::memset(&ev, '\0', sizeof(struct epoll_event));
        while (true)
        {
			ready = epoll_wait(this->_epoll_fd, event, 10, 1000);
			if (g_code)
				return ;
			if (ready == -1)
				throw EpollCreateException();
			for (int i = 0; i < ready; i++)
			{
				if (this->_listen_sockets.find(event[i].data.fd) != this->_listen_sockets.end())
				{
					if (!this->acceptIncomingConnection(event[i]))
						std::cerr << "Cannot accept incoming connection" << std::endl;
				}
				else if (event[i].events == EPOLLIN)
				{
					this->readRequest(event[i]);
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

void	Server::closeTimedoutConnections(void)
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

