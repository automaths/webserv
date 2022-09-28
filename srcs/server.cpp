/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdetune <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/28 13:45:50 by bdetune           #+#    #+#             */
/*   Updated: 2022/09/28 14:38:58 by bdetune          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

Server::Server(void)
{
	return ;
}

Server::~Server(void)
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

void Server::initing(void)
{
	int	_server_fd;

	if ((this->_epoll_fd = epoll_create(1)) == -1)
		throw EpollCreateException();

	if ((_server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) == -1) // creating the socket
		throw SocketCreationException();
	_address.sin_family = AF_INET; // socket configuration
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons( PORT );
	memset(_address.sin_zero, '\0', sizeof(_address.sin_zero)); // applying configurations on the created socket
	if (bind(_server_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0)
		throw BindException();
	if (listen(_server_fd, 10) < 0) // opening the socket to the port
		throw ListenException();
	this->_listen_sockets.insert(std::pair<int, int>(_server_fd, PORT));
}

