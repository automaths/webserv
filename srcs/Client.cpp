/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdetune <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/27 17:13:06 by bdetune           #+#    #+#             */
/*   Updated: 2022/09/29 17:28:55 by bdetune          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(void)
{
	return ;
}

Client::Client(int const & portNumber):_portNumber(portNumber), _request(), _response(), _keepAlive(60), _nbRequest(0), _lastConnection(std::time(NULL))
{
	return ;
}

Client::~Client(void)
{
	return ;
}

Client::Client(Client const & src):_portNumber(src._portNumber), _request(src._request), _response(src._response), _keepAlive(src._keepAlive), _nbRequest(src._nbRequest), _lastConnection(src._lastConnection)
{
	return ;
}

Client & Client::operator=(Client const & rhs)
{
	if (this == &rhs)
		return (*this);
	this->_portNumber = rhs._portNumber;
	this->_request = rhs._request;
	this->_response = rhs._response;
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

Request &	Client::getRequest(void)
{
	return (this->_request);
}

void	Client::resetRequest(void)
{
	Request	resetter;
	this->_request = resetter;
}

Response &	Client::getResponse(void)
{
	this->_lastConnection = std::time(NULL);
	return (this->_response);
}

void	Client::resetResponse(void)
{
	Response	resetter;
	this->_response = resetter;
}

int	Client::addToRequest(std::string packet)
{
	this->_lastConnection = std::time(NULL);
	return (this->_request.parseChunk(packet));
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
