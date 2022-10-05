/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdetune <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/29 12:29:34 by bdetune           #+#    #+#             */
/*   Updated: 2022/10/05 20:52:02 by bdetune          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(void): _header(), _headerSize(), _body(), _bodySize(), _targetFD(), _headerSent(false), _over(false), _FDConsumed(false), _close(false), _targetServer(NULL)
{
	return ;
}

Response::Response(Request & req, std::vector<ServerScope> & matches, int error): _header(), _headerSize(0), _body(), _bodySize(0), _targetFD(0), _headerSent(false), _over(false), _FDConsumed(false), _close(false), _targetServer(NULL)
{
	std::map<std::string, std::list<std::string> >				headerMap = req.getHeaders();
	std::map<std::string, std::list<std::string> >::iterator	host;
	for (std::map<std::string, std::list<std::string> >::iterator st = headerMap.begin(); st != headerMap.end(); st++)
	{
		std::cerr << "Key: " << (*st).first << std::endl;
		for (std::list<std::string>::iterator first = (*st).second.begin(); first != (*st).second.end(); first++)
		{
			std::cerr << "Val: " << *first << std::endl;
		}
	}
	if (error)
		errorResponse(error);
	else
	{
		host = headerMap.find(std::string("host"));
		if (host != headerMap.end())
		{
			std::cerr << "Host" << std::endl;	
			this->getServer(headerMap[std::string("host")].front(), matches);
		}
		else
			this->_targetServer = &(matches.front());
//		basicResponse();
		makeResponse(req);
	}
	return ;
}

bool	Response::findLocation(LocationScope *loc, std::vector<LocationScope> locations, std::string uri)
{
	std::string					path;
	std::size_t					max_depth = 0;
	std::size_t					depth;
	std::string::size_type		index = 0;

	std::cerr << "URI: " << uri << std::endl;
	for (std::vector<LocationScope>::iterator st = locations.begin(); st != locations.end(); st++)
	{
		depth = 0;
		path = st->getMainPath();
		if (path.find(" ") == 0)
			path.erase(0, path.find_first_not_of("\t\n\r\v\f "));
		if (path.find_last_of("\t\n\r\v\f ") == (path.size() - 1))
			path.erase((path.find_last_not_of("\t\n\r\v\f ") + 1));
		std::cerr << "|" << path << "|" << std::endl;
		if (uri.find(path) == 0)
		{
			std::cerr << "Good" << std::endl;
			while ((index = path.find("\\/")) != std::string::npos)
			{
				path.erase(index, 2);
			}
			while ((index = path.find("/")) != std::string::npos)
			{
				path.erase(index, 1);
				depth += 1;
			}
		}
		if (depth > max_depth)
		{
			*loc = *st;
			max_depth = depth;
		}
		else if (depth && depth == max_depth)
		{
			if (st->getMainPath().size() > loc->getMainPath().size())
				*loc = *st;
		}
	}
	if (max_depth && loc->getLocations().size())
		this->findLocation(loc, loc->getLocations(), uri);
	if (max_depth)
		return (true);
	return (false);
}

int	Response::getTargetFD(void)
{
	return (this->_targetFD);
}

bool	Response::bufferResponse(void)
{
	ssize_t				ret;
	std::stringstream	size;
	std::size_t			buff = static_cast<std::size_t>(this->_body.capacity() - this->_body.size());

	if (!buff || (this->_chunked && buff <= 10))
		return (true);
	if (this->_chunked)
		ret = read(this->_targetFD, &(this->_body[this->_body.size()]), (buff - 10));
	else
		ret = read(this->_targetFD, &(this->_body[this->_body.size()]), buff);
	if (ret <= 0)
	{
		if (this->_chunked)
		{
			if (!this->_body.size())
			{
				this->_FDConsumed = true;
				close(this->_targetFD);
			}
			size << std::hex << this->_body.size();
			size <<	"\r\n";
			this->_body = size.str() + this->_body + std::string("\r\n");
		}
		else
		{
			this->_FDConsumed = true;
			close(this->_targetFD);
		}
		this->_bodySize = this->_body.size();
		return (true);
	}
	else if (this->_chunked)
	{
		if ((this->_body.capacity() - this->_body.size()) <= 10)
			return (true);
	}
	return (false);
}

bool	Response::getIsConsumed(void)
{
	return (this->_FDConsumed);
}

void	Response::makeResponse(Request & req)
{
	LocationScope		loc;
	bool				hasLoc = false;
	bool				valid = true;
	std::stringstream	header;
	std::string			fullPath;
	struct stat			buf;

	this->_chunked = false;
	std::cerr << this->_targetServer->getRoot() << std::endl;
	std::vector<std::string>	methods = this->_targetServer->getAllowMethod();
	for (std::vector<std::string>::iterator st = methods.begin(); st != methods.end(); st++)
	{
		std::cerr << *st << std::endl;
	}
	std::vector<LocationScope>	locations = this->_targetServer->getLocations();
	if (locations.size())
	{
		std::cerr << "Looking for location" << std::endl;
		hasLoc = this->findLocation(&loc, locations, req.getFile());
	}
	if (hasLoc)
	{
		std::cerr << "Got location" << std::endl;
		std::vector<std::string>	methods = loc.getAllowMethod();
		if (methods.size())
		{
			valid = false;
			for (std::vector<std::string>::iterator st = methods.begin(); st != methods.end(); st++)
			{
				if (*st == req.getType())
					valid = true;
			}
			if (!valid)
			{
				this->errorResponse(405);
				std::string	st0 = this->_header.substr(0, this->_header.find(std::string("Content-type")));
				header << st0;
				header << "Allow: ";
				for (std::vector<std::string>::size_type i = 0; i < (methods.size() - 1); i++)
					header << methods[i] << ", ";
				header << methods[(methods.size() - 1)] << "\r\n";
				this->_header.erase(0, st0.size());
				header << this->_header;
				this->_header = header.str();
				this->_headerSize = this->_header.size();
				return;
			}
		}
		fullPath = loc.getRoot();
		if (fullPath.find("\t\n\r\v\f ") == 0)
			fullPath.erase(0, fullPath.find_first_not_of("\t\n\r\v\f "));
		if (fullPath.find_last_of("\t\n\r\v\f ") == (fullPath.size() - 1))
			fullPath.erase((fullPath.find_last_not_of("\t\n\r\v\f ") + 1));
		fullPath += req.getFile();
		std::cerr << "Fully qualified path: ***" << fullPath << "***" << std::endl;
		if (access(fullPath.data(), F_OK) == -1)
		{
			this->errorResponse(404);
			return ;
		}
		if (stat(fullPath.data(), &buf) == -1)
		{
			this->errorResponse(404);
			return ;
		}
		std::cerr << "Exists" << std::endl;
		if (S_ISDIR(buf.st_mode))
		{
			std::string	tmpIndex;
			std::cerr << "is directory" << std::endl;
			std::vector<std::string>	indexes = loc.getIndex();
			std::cerr << "Number of potential indexes: " << indexes.size() << std::endl;
			for (std::vector<std::string>::iterator st = indexes.begin(); st != indexes.end(); st++)
			{
				tmpIndex = *st;
				std::cerr << "Index path ***" << tmpIndex << "***" << std::endl;
				if (access((fullPath + *st).data(), F_OK | R_OK) != -1)
				{
					this->_targetFD = open((fullPath + *st).data(), O_RDONLY, O_NONBLOCK);
					if (this->_targetFD > 0)
					{
						if (fstat(this->_targetFD, &buf) == -1 || S_ISDIR(buf.st_mode))
						{
							close(this->_targetFD);
							this->_targetFD = 0;
						}
						else
						{
							if (buf.st_size > 1048576)
							{
								this->_chunked = true;
								this->_bodySize = 1048586;
							}
							else
								this->_bodySize = buf.st_size;
							std::cerr << "Targeted file: " << (fullPath + *st).data() << std::endl;
							break ;
						}
					}
					else
						this->_targetFD = 0;
				}
			}
		}
		else
		{
			this->_targetFD = open(fullPath.data(), O_RDONLY, O_NONBLOCK);
			if (this->_targetFD <= 0)
			{
					close(this->_targetFD);
					this->_targetFD = 0;
					this->errorResponse(404);
					return ;
			}
		}
	}
	else
	{
		std::vector<std::string>	methods = this->_targetServer->getAllowMethod();
		if (methods.size())
		{
			valid = false;
			for (std::vector<std::string>::iterator st = methods.begin(); st != methods.end(); st++)
			{
				if (*st == req.getType())
					valid = true;
			}
			if (!valid)
			{
				this->errorResponse(405);
				std::string	st0 = this->_header.substr(0, this->_header.find(std::string("Content-type")));
				header << st0;
				header << "Allow: ";
				for (std::vector<std::string>::size_type i = 0; i < (methods.size() - 1); i++)
					header << methods[i] << ", ";
				header << methods[(methods.size() - 1)] << "\r\n";
				this->_header.erase(0, st0.size());
				header << this->_header;
				this->_header = header.str();
				this->_headerSize = this->_header.size();
				return;
			}
		}
		fullPath = this->_targetServer->getRoot();
		if (fullPath.find("\t\n\r\v\f ") == 0)
			fullPath.erase(0, fullPath.find_first_not_of("\t\n\r\v\f "));
		if (fullPath.find_last_of("\t\n\r\v\f ") == (fullPath.size() - 1))
			fullPath.erase((fullPath.find_last_not_of("\t\n\r\v\f ") + 1));
		fullPath += req.getFile();
		if (access(fullPath.data(), F_OK) == -1 || stat(fullPath.data(), &buf) == -1)
		{
			this->errorResponse(404);
			return ;
		}
		if (S_ISDIR(buf.st_mode))
		{
			std::string	tmpIndex;
			std::cerr << "is directory no location" << std::endl;
			std::vector<std::string>	indexes = this->_targetServer->getIndex();
			std::cerr << "Number of potential indexes: " << indexes.size() << std::endl;
			for (std::vector<std::string>::iterator st = indexes.begin(); st != indexes.end(); st++)
			{
				tmpIndex = *st;
				std::cerr << "Index path ***" << tmpIndex << "***" << std::endl;
				std::cerr << (fullPath + *st).data() << std::endl;
				if (access((fullPath + *st).data(), F_OK | R_OK) != -1)
				{
					this->_targetFD = open((fullPath + *st).data(), O_RDONLY, O_NONBLOCK);
					if (this->_targetFD > 0)
					{
						if (fstat(this->_targetFD, &buf) == -1 || S_ISDIR(buf.st_mode))
						{
							close(this->_targetFD);
							this->_targetFD = 0;
						}
						else
						{
							if (buf.st_size > 1048576)
							{
								this->_chunked = true;
								this->_bodySize = 1048586;
							}
							else
								this->_bodySize = buf.st_size;
							break ;
						}
					}
					else
						this->_targetFD = 0;
				}
			}
		}
		else
		{
			this->_targetFD = open(fullPath.data(), O_RDONLY, O_NONBLOCK);
			if (this->_targetFD <= 0)
			{
					close(this->_targetFD);
					this->_targetFD = 0;
					this->errorResponse(404);
					return ;
			}
		}
	}
	std::cerr << "off_t: " << sizeof(off_t) << std::endl;
	if (this->_targetFD)
	{
		std::cerr << "Found file: " << fullPath << std::endl;
		header << "HTTP/1.1 200 "<< DEFAULT200STATUS << "\r\n";
		header << setBaseHeader();
		header << "Content-type: text/html\r\n";
		this->_chunked ? (header << "Transfer-Encoding: chunked\r\n") : (header << "Content-Length: " << this->_bodySize << "\r\n");
		header << "Connection: keep-alive\r\n";
		header << "\r\n";
		this->_header = header.str();
		this->_headerSize = this->_header.size();
		this->_body.reserve(this->_bodySize);
	}
	else
	{
		this->errorResponse(404);
		return ;
	}
}

Response::Response(Response const & src): _header(src._header), _headerSize(src._headerSize), _body(src._body), _bodySize(src._bodySize), _targetFD(src._targetFD), _headerSent(src._headerSent), _over(src._over), _close(src._close), _targetServer(src._targetServer)
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
	this->_targetFD = rhs._targetFD;
	this->_headerSent = rhs._headerSent;
	this->_over = rhs._over;
	this->_FDConsumed = rhs._FDConsumed;
	this->_close = rhs._close;
	this->_targetServer = rhs._targetServer;
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
		case 405:
			status << " 405 " << DEFAULT405STATUS;
			body = DEFAULT405BODY;
			break;
		case 416:
			status << " 416 " << DEFAULT416STATUS;
			body = DEFAULT416BODY;
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

bool	Response::getServer(std::string const & host, std::vector<ServerScope> & matches)
{
	std::vector<std::string>	lst;
	std::vector<std::string>::iterator	first;

	for (std::vector<ServerScope>::iterator st = matches.begin(); st != matches.end(); st++)
	{
		lst = st->getServerName();
		for (std::vector<std::string>::iterator first = lst.begin(); first != lst.end(); first++)
		{
			if (*first == host)
			{
				this->_targetServer = &(*st);
				std::cerr << "Found Server: " << *first << std::endl;
				return (true);
			}
		}
	}
	this->_targetServer = &(matches.front());
	return (false);
}
