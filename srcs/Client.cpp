/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdetune <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/27 17:13:06 by bdetune           #+#    #+#             */
/*   Updated: 2022/09/27 20:30:31 by bdetune          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(void)
{
	return ;
}

Client::Client(int const & portNumber):_portNumber(portNumber), _request(), _keepAlive(60), _nbRequest(0), _lastConnection(std::time(NULL))
{
	return ;
}

Client::~Client(void)
{
	return ;
}

Client::Client(Client const & src):_portNumber(src._portNumber), _request(src._request), _keepAlive(src._keepAlive), _nbRequest(src._nbRequest), _lastConnection(src._lastConnection)
{
	return ;
}

Client & Client::operator=(Client const & rhs)
{
	if (this == &rhs)
		return (*this);
	this->_portNumber = rhs._portNumber;
	this->_request = rhs._request;
	this->_keepAlive = rhs._keepAlive;
	this->_nbRequest = rhs._nbRequest;
	this->_lastConnection = rhs._lastConnection;
	return (*this);
}

int &	Client::getPortNumber(void)
{
	return (this->_portNumber);
}

void	Client::setPortNumber(int portNumber)
{
	this->_portNumber = portNumber;
}

std::string &	Client::getRequest(void)
{
	return (this->_request);
}

void	Client::setRequest(std::string const & request)
{
	this->_request = request;
	this->_lastConnection = std::time(NULL);
}

void	Client::addToRequest(std::string const & packet)
{
	this->_request += packet;
	this->_lastConnection = std::time(NULL);
}

int &	Client::getKeepAlive(void)
{
	return (this->_keepAlive);
}

void	Client::setKeepAlive(int keepAlive)
{
	this->_keepAlive = keepAlive;
}

unsigned int	Client::getNbRequest(void)
{
	return (this->_nbRequest);
}

void	Client::addNbRequest(void)
{
	this->_nbRequest++;
}
std::time_t	Client::getLastConnection(void)
{
	return (this->_lastConnection);
}
