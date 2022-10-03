/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdetune <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/28 13:45:50 by bdetune           #+#    #+#             */
/*   Updated: 2022/10/03 15:55:46 by bdetune          ###   ########.fr       */
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

void	Server::acceptIncomingConnection(struct epoll_event & event)
{
	Client				tmp(this->_listen_sockets[event.data.fd]);
	struct epoll_event	ev;
    int					addrlen	=sizeof(_address);

	std::memset(&ev, '\0', sizeof(struct epoll_event));
	ev.events = EPOLLIN;
	ev.data.fd = accept(event.data.fd, (struct sockaddr *)&_address, (socklen_t*)&addrlen);
	if (ev.data.fd < 0)
	{
		std::cerr << "Could not accept incoming connection" << std::endl;
		return ;
	}
	if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, ev.data.fd, &ev) == -1)
	{
		std::cerr << "Error while adding fd to epoll" << std::endl;
		close(ev.data.fd);
		return ;
	}
	try
	{
		this->_client_sockets.insert(std::pair<int, Client>(ev.data.fd, tmp));
	}
	catch (std::exception const & e)
	{
		std::cerr << "Could not insert new client in map: " << e.what() << std::endl;
		epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, ev.data.fd, &ev);
		close(ev.data.fd);
	}
}

void	Server::readRequest(struct epoll_event & event)
{
	int		result;
	ssize_t	recvret = 0;

	recvret = recv(event.data.fd, static_cast<void *>(_rdBuffer), 1048576, MSG_DONTWAIT);
	if (recvret == -1)
		std::cerr << "Error while trying to read socket" << std::endl;
	else if (recvret == 0)
	{
		std::cerr << "Client closed connection while receiving data" << std::endl;
		this->closeClientSocket(event);
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
		if (result == 200)
		{
			this->_client_sockets[event.data.fd].getResponse() = Response(this->_client_sockets[event.data.fd].getRequest());
			event.events = EPOLLOUT;
			if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_MOD, event.data.fd, &event) == -1)
			{
				this->closeClientSocket(event);
				std::cerr << "Error Trying to switch socket from reading to writing" << std::endl;
			}
		}
		else if (result)
		{
			std::cerr << "Parsing result: " << result << std::endl;
			if (result == 1)
			{
				this->closeClientSocket(event);
				return ;
			}
			this->_client_sockets[event.data.fd].getResponse() = Response(this->_client_sockets[event.data.fd].getRequest(), result);
			event.events = EPOLLOUT;
			if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_MOD, event.data.fd, &event) == -1)
			{
				this->closeClientSocket(event);
				std::cerr << "Error Trying to switch socket from reading to writing" << std::endl;
			}
		}
	}
}

bool	Server::sendHeader(struct epoll_event & event)
{
	ssize_t	sendret = 0;

	sendret = send(event.data.fd, this->_client_sockets[event.data.fd].getResponse().getHeader().data(), this->_client_sockets[event.data.fd].getResponse().getHeaderSize(), MSG_NOSIGNAL | MSG_DONTWAIT);
	if (sendret == -1)
	{
		std::cerr << "Error while sending" << std::endl;
		this->closeClientSocket(event);
		return (false) ;
	}
	return (this->_client_sockets[event.data.fd].getResponse().headerBytesSent(sendret));
}

void	Server::sendBody(struct epoll_event & event)
{
	ssize_t	sendret = 0;

	sendret = send(event.data.fd, this->_client_sockets[event.data.fd].getResponse().getBody().data(), this->_client_sockets[event.data.fd].getResponse().getBodySize(), MSG_NOSIGNAL | MSG_DONTWAIT);
	if (sendret == -1)
	{
		std::cerr << "Error while sending" << std::endl;
		this->closeClientSocket(event);
		return ;
	}
	if (this->_client_sockets[event.data.fd].getResponse().bodyBytesSent(sendret))
	{
		if (this->_client_sockets[event.data.fd].getResponse().getClose())
			this->closeClientSocket(event);
		else
		{
			this->_client_sockets[event.data.fd].resetRequest();
			this->_client_sockets[event.data.fd].resetResponse();
			event.events = EPOLLIN;
			if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_MOD, event.data.fd, &event))
			{
				std::cerr << "Error switching form write to read mode" << std::endl;
				this->closeClientSocket(event);
			}
		}
	}
}

void	Server::closeClientSocket(struct epoll_event & event)
{
	epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
	close(event.data.fd);
	this->_client_sockets.erase(event.data.fd);
}

void	Server::sendResponse(struct epoll_event & event)
{
	std::cerr << "Sending" << std::endl;
	if (!this->_client_sockets[event.data.fd].getResponse().headerIsSent())
	{
		if (this->sendHeader(event))
			this->sendBody(event);
	}
	else
		this->sendBody(event);
}

void Server::execute(void)
{
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
			continue ;
		for (int i = 0; i < ready; i++)
		{
			if (this->_listen_sockets.find(event[i].data.fd) != this->_listen_sockets.end())
				this->acceptIncomingConnection(event[i]);
			else if (event[i].events == EPOLLIN)
				this->readRequest(event[i]);
			else if (event[i].events == EPOLLOUT)
				this->sendResponse(event[i]);
			this->closeTimedoutConnections();
        }
		std::cout << "The number of clients is: " << this->_client_sockets.size() << std::endl;
    }
}

void	Server::closeTimedoutConnections(void)
{
		std::time_t	current = std::time(NULL);
		std::map<int, Client>::iterator st = this->_client_sockets.begin();
		std::map<int, Client>::iterator tmp;
		struct epoll_event	ev;

		std::memset(&ev, '\0', sizeof(struct epoll_event));
		while (st!= this->_client_sockets.end())
		{
			tmp = st;
			tmp++;
			if (std::difftime(current, (*st).second.getLastConnection()) > (*st).second.getKeepAlive())
			{
				ev.data.fd = (*st).first;
				this->closeClientSocket(ev);
			}
			st = tmp;
		}
	}

