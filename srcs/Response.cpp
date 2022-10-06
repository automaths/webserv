/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsartral <nsartral@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/29 12:29:34 by bdetune           #+#    #+#             */
/*   Updated: 2022/10/06 21:18:50 by nsartral         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(void): _header(), _headerSize(), _body(), _bodySize(), _targetFile(), _targetFilePath(""), _headerSent(false), _over(false), _fileConsumed(false), _close(false), _targetServer(NULL), _responseType(0)
{
	return ;
}

std::vector<std::string> parseEnv(Request & req) {
	std::vector<std::string>	env;
	std::map<std::string, std::list<std::string> > map = req.getHeaders();
	std::map<std::string, std::list<std::string> >::iterator tmp = map.begin();
	std::string					val;

	env.push_back("REQUEST_METHOD=" + req.getType());
	env.push_back("SERVER_PROTOCOL=" + req.getVersion());
	while (tmp != map.end()) {
		val = "";
		std::list<std::string>::iterator i = (*tmp).second.begin(); 
		while (i != (*tmp).second.end()) {
			val += *i;
			i++;
			if (i != (*tmp).second.end())
				val += ",";
		}
		env.push_back("HTTP_" + (*tmp).first + "=" + val);
		tmp++;
	}
	return (env);	
}

Response::Response(Request & req, std::vector<ServerScope> & matches, int error): _header(), _headerSize(0), _body(), _bodySize(0), _targetFile(), _targetFilePath(""), _headerSent(false), _over(false), _fileConsumed(false), _close(false), _targetServer(NULL), _responseType(0)
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

bool	Response::getIsConsumed(void)
{
	return (this->_fileConsumed);
}

bool	Response::allowedMethod(std::vector<std::string> methods, std::string currentMethod)
{
	std::stringstream	header;

	if (!methods.size())
		return (true);
	for (std::vector<std::string>::iterator st = methods.begin(); st != methods.end(); st++)
	{
		if (*st == currentMethod)
			return (true);
	}
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
	return (false);
}

bool	Response::pathIsValid(std::string path, struct stat * buf)
{
	if (access(path.data(), F_OK) == -1)
		return (false);
	if (stat(path.data(), buf) == -1)
		return (false);
	return (true);
}

bool	Response::foundDirectoryIndex(std::vector<std::string> indexes, std::string path)
{
	struct stat	buf;
	std::string	tmpIndex;

	if (path.size() == 0)
		path = "/";
	else if (path[path.size() - 1] != '/')
		path += "/";
	for (std::vector<std::string>::iterator st = indexes.begin(); st != indexes.end(); st++)
	{
		tmpIndex = path + *st;
		std::cerr << "Index path ***" << tmpIndex << "***" << std::endl;
		if (access(tmpIndex.data(), F_OK | R_OK) != -1)
		{
			if (stat(tmpIndex.data(), &buf) == -1 || S_ISDIR(buf.st_mode) || !S_ISREG(buf.st_mode))
				continue ;
			this->_targetFile.open(tmpIndex.data(), std::ifstream::binary);
			if (this->_targetFile.fail())
				continue ;
			if (buf.st_size > 1048576)
			{
				this->_chunked = true;
				this->_bodySize = 1048586;
			}
			else
				this->_bodySize = buf.st_size;
			this->_targetFilePath = tmpIndex;
			std::cerr << "Targeted file: " << tmpIndex.data() << std::endl;
			return (true);
		}
	}
	return (false);
}

void	Response::createFileResponse(void)
{
	std::string			extension = "";
	std::stringstream	header;
	std::stringstream	size;

	if (this->_targetFilePath.find_last_of(".") != std::string::npos)
		extension = this->_targetFilePath.substr(this->_targetFilePath.find_last_of("."));
	extension = MimeTypes().convert(extension);
	this->_responseType = 2;
	this->_body.reserve((this->_bodySize + 1));
	if (this->_chunked)
	{
		this->_targetFile.read(&(this->_body[0]), 1048576);
		if (!this->_targetFile.eof() && this->_targetFile.fail())
		{
			this->errorResponse(500);
			return ;
		}
		this->_bodySize = this->_targetFile.gcount();
		this->_fileConsumed = false;
		size << std::hex << this->_bodySize;
		size <<	"\r\n";
		this->_body = size.str() + this->_body + std::string("\r\n");
		this->_bodySize += size.str().size() + 2;
	}
	else
	{
		this->_targetFile.read(&(this->_body[0]), (this->_bodySize + 1));
		if (!this->_targetFile.eof() && this->_targetFile.fail())
		{
			this->errorResponse(500);
			return ;
		}
		this->_bodySize = this->_targetFile.gcount();
		if (this->_targetFile.eof())
		{
			std::cerr << "EOF" << std::endl;
			this->_fileConsumed = true;
			this->_targetFile.close();
		}
		else
			this->_fileConsumed = false;
	}
	header << "HTTP/1.1 200 "<< DEFAULT200STATUS << "\r\n";
	header << setBaseHeader();
	header << "Content-type: " << extension << "\r\n";
	this->_chunked ? (header << "Transfer-Encoding: chunked\r\n") : (header << "Content-Length: " << this->_bodySize << "\r\n");
	header << "Connection: keep-alive\r\n";
	header << "\r\n";
	this->_header = header.str();
	this->_headerSize = this->_header.size();
}

void	Response::makeResponse(Request & req)
{
	LocationScope		loc;
	bool				hasLoc = false;
	std::string			fullPath;
	struct stat			buf;

	if (this->_responseType == 1)
		return ;
	_env = parseEnv(req);
	this->_chunked = false;
	// std::cerr << this->_targetServer->getRoot() << std::endl;
	std::vector<LocationScope>	locations = this->_targetServer->getLocations();
	if (locations.size())
		hasLoc = this->findLocation(&loc, locations, req.getFile());
	if ((hasLoc && !allowedMethod(loc.getAllowMethod(), req.getType())) || (!hasLoc && !allowedMethod(this->_targetServer->getAllowMethod(), req.getType())))
		return;
	if (hasLoc)
		fullPath = loc.getRoot();
	else
		fullPath = this->_targetServer->getRoot();
	if (fullPath.find("\t\n\r\v\f ") == 0)
		fullPath.erase(0, fullPath.find_first_not_of("\t\n\r\v\f "));
	if (fullPath.find_last_of("\t\n\r\v\f ") == (fullPath.size() - 1))
		fullPath.erase((fullPath.find_last_not_of("\t\n\r\v\f ") + 1));
	if (hasLoc)
	{
		std::string	partial_root = req.getFile();
		partial_root.erase(0, loc.getMainPath().size());
		fullPath += partial_root;
	}
	else
		fullPath += req.getFile();
	std::cerr << "Fully qualified path: ***" << fullPath << "***" << std::endl;
	if (!pathIsValid(fullPath, &buf))
	{
		this->errorResponse(404);
		return ;
	}
	// std::cerr << "Exists" << std::endl;
	if (S_ISDIR(buf.st_mode))
	{
//		std::cerr << "is directory" << std::endl;
		std::vector<std::string>	indexes;
		if (hasLoc)
			indexes = loc.getIndex();
		else
			indexes = this->_targetServer->getIndex();
//		std::cerr << "Number of potential indexes: " << indexes.size() << std::endl;
		if (foundDirectoryIndex(indexes, fullPath))
		{
			this->createFileResponse();
			return;
		}
/*		else
		{
			List files in directory
			$uri = req.getFile();
	
			<!DOCTYPE html>
			<html lang="en">
			<head>
			<meta charset="UTF-8">
			<meta http-equiv="X-UA-Compatible" content="IE=edge">
			<meta name="viewport" content="width=device-width, initial-scale=1.0">
			<title>Index $uri</title>
			</head>
			<body>
				<h1>$uri</h1>
				<ul>
					<li><a href="/$uri/file1">file1</a></li>
					<li><a href="/$uri/file2">file2</a></li>
				</ul>
			</body>
			</html>			
		} */
	}
	else if (S_ISREG(buf.st_mode))
	{
		this->_targetFile.open(fullPath.data(), std::ifstream::binary);
		if (!this->_targetFile.fail())
		{
			if (buf.st_size > 1048576)
			{
				this->_chunked = true;
				this->_bodySize = 1048586;
			}
			else
				this->_bodySize = buf.st_size;
			this->_targetFilePath = fullPath;
			this->createFileResponse();
			return;
		}			
	}
	this->errorResponse(404);
}

Response::Response(Response const & src): _header(src._header), _headerSize(src._headerSize), _body(src._body), _bodySize(src._bodySize), _headerSent(src._headerSent), _over(src._over), _fileConsumed(src._fileConsumed), _close(src._close), _targetServer(src._targetServer), _responseType(src._responseType)
{
	return ;
}

std::vector<std::string> Response::getEnv(void) const {
	return (_env);
}

Response::~Response(void)
{
	return ;
}

Response &	Response::operator=(Response const & rhs)
{
	if (&rhs == this)
		return (*this);
	if (this->_targetFile.is_open())
		this->_targetFile.close();
	this->_header = rhs._header;
	this->_headerSize = rhs._headerSize;
	this->_body = rhs._body;
	this->_bodySize = rhs._bodySize;
	this->_headerSent = rhs._headerSent;
	this->_over = rhs._over;
	this->_fileConsumed = rhs._fileConsumed;
	this->_close = rhs._close;
	this->_targetServer = rhs._targetServer;
	this->_responseType = rhs._responseType;
	return (*this);
}

void	Response::errorResponse(int error)
{
	std::stringstream	header;
	std::stringstream	status;
	std::string			body;

	this->_responseType = 1;
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
	std::stringstream	size;

	if (this->_bodySize <= bytes)
	{
		this->_body.clear();
		if (this->_responseType == 2 && this->_chunked && !this->_fileConsumed && this->_targetFile.is_open())
		{
			this->_targetFile.read(&(this->_body[0]), 1048576);
			this->_bodySize = this->_targetFile.gcount();
			if (this->_targetFile.eof())
			{
				if (this->_bodySize == 0)
					this->_fileConsumed = true;
			}
			else if (this->_targetFile.fail())
			{
				this->_over = true;
				return (true);
			}
			size << std::hex << this->_bodySize;
			size <<	"\r\n";
			this->_body = size.str() + this->_body + std::string("\r\n");
			this->_bodySize += size.str().size() + 2;
		}
		else if (this->_responseType == 2 && !this->_fileConsumed && this->_targetFile.is_open())
		{
			this->_targetFile.read(&(this->_body[0]), 1048576);
			if (this->_targetFile.eof())
				this->_fileConsumed = true;
			else if (this->_targetFile.fail())
			{
				this->_over = true;
				return (true);
			}
			this->_bodySize = this->_targetFile.gcount();
		}
		else
			this->_over = true;
	}
	else
	{
		this->_body.assign(this->_body[bytes], (this->_bodySize - bytes));
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
