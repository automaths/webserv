/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdetune <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/29 12:29:34 by bdetune           #+#    #+#             */
/*   Updated: 2022/10/03 14:08:41 by bdetune          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(void): _header(), _headerSize(), _body(), _bodySize(), _headerSent(false), _over(false), _close(false)
{
	return ;
}

Response::Response(Request & req, int error): _header(), _headerSize(), _body(), _bodySize(), _headerSent(false), _over(false), _close(false)
{
	if (error && error != 1)
		errorResponse(error);
	else if (error)
	{
		this->_header = "";
		this->_headerSize = 0;
		this->_body = "";
		this->_bodySize = 0;
	}
	else
		basicResponse();
	(void)req;
	return ;
}

Response::Response(Response const & src): _header(src._header), _headerSize(src._headerSize), _body(src._body), _bodySize(src._bodySize), _headerSent(src._headerSent), _over(src._over), _close(src._close)
{
	return ;
}

Response::~Response(void)
{
	return ;
}

Response &	Response::operator=(Response const & rhs)
{
	if (&rhs == this)
		return (*this);
	this->_header = rhs._header;
	this->_headerSize = rhs._headerSize;
	this->_body = rhs._body;
	this->_bodySize = rhs._bodySize;
	this->_headerSent = rhs._headerSent;
	this->_over = rhs._over;
	this->_close = rhs._close;
	return (*this);
}

void	Response::errorResponse(int error)
{
	std::stringstream	header;
	std::stringstream	status;
	std::string			body;

	this->_close = true;
	switch (error)
	{
		case 400:
			status << " 400 " << DEFAULT400STATUS;
			body = DEFAULT400BODY;
			break ;
		case 401:
			status << " 401 " << DEFAULT401STATUS;
			body = DEFAULT401BODY;
			break ;
		case 403:
			status << " 403 " << DEFAULT403STATUS;
			body = DEFAULT403BODY;
			break ;
		case 404:
			status << " 404 " << DEFAULT404STATUS;
			body = DEFAULT404BODY;
			break;
		case 505:
			status << " 505 " << DEFAULT505STATUS;
			body = DEFAULT505BODY;
			break ;
		default:
			header << "HTTP/1.1 400 " << "Oupsy daisy" << "\r\n";
			this->_header = header.str();
			break;
	}

	header << "HTTP/1.1" << status.str() << "\r\n";
	header << setBaseHeader();
	header << "Content-type: text/html\r\n";
	header << "Content-Length: " << body.size() << "\r\n";
	header << "Connection: close\r\n";
	header << "\r\n";
	this->_header = header.str();
	this->_headerSize = this->_header.size();
	this->_body = body;
	this->_bodySize = this->_body.size();
}

void	Response::basicResponse(void)
{
	std::stringstream	header;
	
	this->_body = DEFAULT200BODY;
	this->_bodySize = this->_body.size();
	header << "HTTP/1.1 200 "<< DEFAULT200STATUS << "\r\n";
	header << setBaseHeader();
	header << "Content-type: text/html\r\n";
	header << "Content-Length: " << this->_bodySize << "\r\n";
	header << "Connection: keep-alive\r\n";
	header << "\r\n";
	this->_header = header.str();
	this->_headerSize = this->_header.size();
}

std::string	Response::setBaseHeader(void)
{
	std::locale					loc;
	std::time_t					now = std::time(NULL);
	std::tm*					gmt = std::gmtime(&now);
	std::string					pattern("%a, %d %b %Y %H:%M:%S");
	const std::time_put<char>	&tmput = std::use_facet <std::time_put<char> > (loc);
	std::stringstream			baseHeader;
	
	baseHeader << "Server: webserv/1.0\r\n";
	baseHeader << "Date: ";
	tmput.put(baseHeader, baseHeader, ' ', gmt, pattern.data(), pattern.data()+pattern.length());
	baseHeader << " GMT\r\n";
	return (baseHeader.str());
}

std::string & Response::getHeader(void)
{
	return (this->_header);
}

std::string & Response::getBody(void)
{
	return (this->_body);
}

std::size_t Response::getHeaderSize(void)
{
	return (this->_headerSize);
}

std::size_t Response::getBodySize(void)
{
	return (this->_bodySize);
}

bool	Response::headerIsSent(void)
{
	return (this->_headerSent);
}

bool	Response::isOver(void)
{
	return (this->_over);
}

bool	Response::headerBytesSent(std::size_t bytes)
{
	if (this->_header.size() <= bytes)
	{
		this->_header.clear();
		this->_headerSent = true;
	}
	else
	{
		this->_header.erase(0, bytes);
		this->_headerSize -= bytes;
	}
	return (this->_headerSent);
}

bool	Response::bodyBytesSent(std::size_t bytes)
{
	if (this->_bodySize <= bytes)
	{
		this->_body.clear();
		this->_over = true;
	}
	else
	{
		this->_body.erase(0, bytes);
		this->_bodySize -= bytes;
	}
	return (this->_over);
}

bool	Response::getClose(void)
{
	return (this->_close);
}
