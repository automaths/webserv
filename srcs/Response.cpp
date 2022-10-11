/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsartral <nsartral@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/29 12:29:34 by bdetune           #+#    #+#             */
/*   Updated: 2022/10/11 15:15:23 by bdetune          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "cgi.hpp"
#include <ext/stdio_filebuf.h>
#include <fstream>
#include <istream>
#include <iostream>

Response::Response(void): _header(), _headerSize(), _body(), _bodySize(), _targetFile(), _targetFilePath(""), _headerSent(false), _over(false), _fileConsumed(false), _close(false), _targetServer(NULL), _responseType(0), _is_cgi(false), _cgi_fd(-1)
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

Response::Response(Request & req, std::vector<ServerScope> & matches, int error): _header(), _headerSize(0), _body(), _bodySize(0), _targetFile(), _targetFilePath(""), _headerSent(false), _over(false), _fileConsumed(false), _close(false), _targetServer(NULL), _responseType(0), _is_cgi(false), _cgi_fd(-1)
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

bool	Response::isCgi(void)
{
	return (this->_is_cgi);
}

int	Response::getCgiFd(void)
{
	return (this->_cgi_fd);
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

bool	Response::openFile(std::string path)
{
	struct stat	buf;

	if (access(path.data(), F_OK | R_OK) == -1)
		return (false);
	if (stat(path.data(), &buf) == -1 || S_ISDIR(buf.st_mode) || !S_ISREG(buf.st_mode))
		return (false) ;
	this->_targetFile.open(path.data(), std::ifstream::binary);
	if (this->_targetFile.fail())
		return (false) ;
	if (buf.st_size > 1048576)
	{
		this->_chunked = true;
		this->_bodySize = 1048586;
	}
	else
		this->_bodySize = buf.st_size;
	this->_targetFilePath = path;
	std::cerr << "Targeted file: " << path << std::endl;
	return (true);
}

bool	Response::foundDirectoryIndex(std::vector<std::string> indexes, std::string path)
{
	std::string	tmpIndex;

	if (path.size() == 0)
		path = "/";
	else if (path[path.size() - 1] != '/')
		path += "/";
	for (std::vector<std::string>::iterator st = indexes.begin(); st != indexes.end(); st++)
	{
		tmpIndex = path + *st;
		std::cerr << "Index path ***" << tmpIndex << "***" << std::endl;
		if (this->openFile(tmpIndex))
			return (true);
	}
	return (false);
}

int Response::execCgi(std::string exec)
{
	_env.push_back("SCRIPT_FILENAME=" + _targetFilePath);
	_env.push_back("SERVER_PORT=" + _targetServer->getPort());
    _env.push_back("PATH_INFO=" + exec);
    _env.push_back("REDIRECT_STATUS=1");
    Cgi test(_targetFilePath, exec, _env, _cgi_input);
	return test.getResult();
}

void Response::cgiResponse(int fd)
{
	int size;
	char buffer[1048576];
	memset(buffer, 0, 1048576);
	size = read(fd, &buffer, 1048576);
	if (size == 0)
	{
		std::cerr << "END OF PIPE" << std::endl;
		_fileConsumed = true;
		_body = "0\r\n\r\n";
		_bodySize = 5;
		std::cout << "read is 0" << std::endl;
		return;
	}
	std::string extension;
	if (this->_targetFilePath.find_last_of(".") != std::string::npos)
		extension = this->_targetFilePath.substr(this->_targetFilePath.find_last_of("."));
	if (_headerSent)
	{
		_body.clear();
		std::stringstream hexsize;
		hexsize << std::hex << size << "\r\n";
		_body += hexsize.str();
		_body += buffer;
	}
	else
	{
		_body = buffer;
		std::string cgiHeader = _body.substr(0, _body.find_first_of("\r\n\r\n") + 2);
		std::cout << "the cgi header is: \n" << cgiHeader << std::endl;
		std::cout << "end" << std::endl;
		_body.erase(0, _body.find_first_of("\r\n\r\n") + 4);
		// _body += buffer;
		_bodySize = _body.size();
		std::stringstream	header;
		header << "HTTP/1.1 200 "<< DEFAULT200STATUS << "\r\n";
		header << setBaseHeader();
		header << cgiHeader;
		if (cgiHeader.find("Content-type:") == std::string::npos)
			header << "Content-type: " << MimeTypes().convert(extension) << "\r\n";
		// header << "Transfer-Encoding: chunked\r\n";
		header << "Content-size: " << _body.size() + cgiHeader.size() << "\r\n";
		header << "Connection: keep-alive\r\n";
		header << "\r\n";
		this->_header = header.str();
		this->_headerSize = this->_header.size();

		std::cout << "the header is :" << header.str() << std::endl;
		std::cout << "the body is: " << _body << std::endl;
	}
	memset(buffer, 0, 1048576);
}

std::string	Response::createFileResponse(void)
{
	std::string			extension = "";
	std::stringstream	size;

	if (this->_targetFilePath.find_last_of(".") != std::string::npos)
		extension = this->_targetFilePath.substr(this->_targetFilePath.find_last_of("."));
	//cgi implementation
	_is_cgi = 0;
	_cgi_fd = -1;
	_cgi_input = -1; //to set to the target file in the case of a POST or PUT
	std::map<std::string, std::string> cgi = _targetServer->getCgi(); 
	for (std::map<std::string, std::string>::iterator it = cgi.begin(); it != cgi.end(); ++it)
	{
		if (!extension.compare(it->first))
		{
			std::cout << "extension " << extension << " match the config extension " << it->first << " associated to path " << it->second << std::endl;
			_is_cgi = 1;
			_cgi_fd = execCgi(it->second);
			// cgiResponse(_cgi_fd);
			return (extension);
		}
	}
	//end of implementation
	extension = MimeTypes().convert(extension);
	this->_responseType = 2;
	this->_body.reserve((this->_bodySize + 2));
	if (this->_chunked)
	{
		this->_targetFile.read(&(this->_body[0]), 1048576);
		if (!this->_targetFile.eof() && this->_targetFile.fail())
		{
			this->_responseType = 1;
			return (extension);
		}
		this->_bodySize = this->_targetFile.gcount();
		std::cerr << "Chunk size:" << this->_bodySize << std::endl;
		this->_fileConsumed = false;
		for (int i = 0; i < 15 ; i++)
		{
			std::cout << this->_body[i];
		}
		std::cout << std::endl;
		size << std::hex << this->_bodySize;
		size <<	"\r\n";
		std::cerr << "HEX size " << size.str() << std::endl;
		std::string	hex_size = size.str();
		std::size_t	hex_length = size.str().size();
		std::size_t	cpas = 0;
		for (std::size_t j = this->_bodySize; j > 0; j--)
		{
			this->_body[j - 1 + hex_length] = this->_body[j - 1];
		}
		for (std::string::iterator st = hex_size.begin(); st != hex_size.end(); st++, cpas++)
		{
			this->_body[cpas] = *st;
		}
		for (int i = 0; i < 15 ; i++)
		{
			std::cout << this->_body[i];
		}
		this->_bodySize += hex_length;
		std::cout << std::endl;
		this->_body[this->_bodySize] = '\r';
		this->_body[this->_bodySize + 1] = '\n';
		this->_bodySize +=  2;
		std::cerr << "Chunk size " << this->_bodySize << std::endl;
	}
	else
	{
		this->_targetFile.read(&(this->_body[0]), (this->_bodySize + 1));
		if (!this->_targetFile.eof() && this->_targetFile.fail())
		{
			this->_responseType = 1;
			return (extension);
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
	return (extension);
}

void	Response::create200Header(std::string extension)
{
	std::stringstream	header;

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
	std::string			extension;

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
		if (hasLoc && loc.getDefaultErrorPage().find("404") != loc.getDefaultErrorPage().end())
			fullPath = (loc.getDefaultErrorPage())["404"];
		else if (!hasLoc && this->_targetServer->getDefaultErrorPage().find("404") != this->_targetServer->getDefaultErrorPage().end())
			fullPath = (this->_targetServer->getDefaultErrorPage())["404"];
		else
		{
			this->errorResponse(404);
			return ;
		}
		_targetFilePath = fullPath;
		if (!this->openFile(fullPath))
		{
			this->errorResponse(404);
			return ;
		}
		extension = this->createFileResponse();
		if (_is_cgi)
			return ;
		if (this->_responseType == 1)
		{
			this->errorResponse(500);
			return ;
		}
		this->createFileErrorHeader(404, extension);
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
			extension = this->createFileResponse();
			if (_is_cgi)
				return ;
			if (this->_responseType == 1)
			{
				this->errorResponse(500);
				return ;
			}
			this->create200Header(extension);
			return;
		}
		else
		{
			try
			{
				ListDirectory listing(fullPath, req.getFile());
				_body = listing.listing();
				_bodySize = _body.size();
	
				std::cout << "the full path is: " << fullPath << std::endl;
				std::cout << "the body is: " << _body << std::endl;
	
				std::stringstream header;
				header << "HTTP/1.1 200 "<< DEFAULT200STATUS << "\r\n";
				header << setBaseHeader();
				header << "Content-type: text/html" << "\r\n";
				header << "Content-Length: " << this->_bodySize << "\r\n";
				header << "Connection: keep-alive\r\n";
				header << "\r\n";
				this->_header = header.str();
				this->_headerSize = this->_header.size();
			}
			catch (std::exception const & src)
			{
				this->errorResponse(500);
			}
			return ;
		}
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
			extension = this->createFileResponse();
			if (_is_cgi)
				return ;
			if (this->_responseType == 1)
			{
				this->errorResponse(500);
				return ;
			}
			this->create200Header(extension);
			return;
		}			
	}
	if (hasLoc && loc.getDefaultErrorPage().find("404") != loc.getDefaultErrorPage().end())
		fullPath = (loc.getDefaultErrorPage())["404"];
	else if (!hasLoc && this->_targetServer->getDefaultErrorPage().find("404") != this->_targetServer->getDefaultErrorPage().end())
		fullPath = (this->_targetServer->getDefaultErrorPage())["404"];
	else
	{
		this->errorResponse(404);
		return ;
	}
	_targetFilePath = fullPath;
	if (!this->openFile(fullPath))
	{
		this->errorResponse(404);
		return ;
	}
	extension = this->createFileResponse();
	if (_is_cgi)
		return ;
	if (this->_responseType == 1)
	{
		this->errorResponse(500);
		return ;
	}
	this->createFileErrorHeader(404, extension);
}

Response::Response(Response const & src): _header(src._header), _headerSize(src._headerSize), _body(src._body), _bodySize(src._bodySize), _headerSent(src._headerSent), _over(src._over), _fileConsumed(src._fileConsumed), _close(src._close), _targetServer(src._targetServer), _responseType(src._responseType), _is_cgi(src._is_cgi), _cgi_fd(src._cgi_fd)
{
	return ;
}

std::vector<std::string> Response::getEnv(void) const {
	return (_env);
}

Response::~Response(void)
{
	if (_is_cgi)
	{
		if (fcntl(_cgi_fd, F_GETFD) != -1)
			close (_cgi_fd);
	}
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
	this->_is_cgi = rhs._is_cgi;
	this->_cgi_fd = rhs._cgi_fd;
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
		case 500:
			status << " 500 " << DEFAULT500STATUS;
			body = DEFAULT500BODY;
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

void	Response::createFileErrorHeader(int errorCode, std::string mime)
{
	std::stringstream	header;
	std::stringstream	status;
	std::string			body;

	this->_close = true;
	switch (errorCode)
	{
		case 400:
			status << " 400 " << DEFAULT400STATUS;
			break ;
		case 401:
			status << " 401 " << DEFAULT401STATUS;
			break ;
		case 403:
			status << " 403 " << DEFAULT403STATUS;
			break ;
		case 404:
			status << " 404 " << DEFAULT404STATUS;
			break;
		case 405:
			status << " 405 " << DEFAULT405STATUS;
			break;
		case 416:
			status << " 416 " << DEFAULT416STATUS;
			break;
		case 505:
			status << " 505 " << DEFAULT505STATUS;
			break ;
		default:
			header << "HTTP/1.1 400 " << "Oupsy daisy" << "\r\n";
			this->_header = header.str();
			break;
	}
	header << "HTTP/1.1" << status.str() << "\r\n";
	header << setBaseHeader();
	header << "Content-type: " << mime << "\r\n";
	this->_chunked ? (header << "Transfer-Encoding: chunked\r\n") : (header << "Content-Length: " << this->_bodySize << "\r\n");
	header << "Connection: close\r\n";
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

	std::cerr << "bytes sent: " << bytes << ", body size: " << this->_bodySize << std::endl;
	if (this->_bodySize <= bytes)
	{
		this->_body.clear();
		this->_bodySize = 0;
		if (this->_is_cgi && !this->_fileConsumed )
		{
			return (false);
		}
		else if (this->_is_cgi && this->_fileConsumed)
		{
			this->_over = true;
			return (true);
		}
		else if (this->_responseType == 2 && this->_chunked && this->_fileConsumed && !this->_over)
		{
			this->_body = std::string("0\r\n\r\n");
			this->_bodySize = 5;
		}
		else if (this->_responseType == 2 && this->_chunked && !this->_fileConsumed && this->_targetFile.is_open())
		{
			this->_targetFile.read(&(this->_body[0]), 1048576);
			this->_bodySize = this->_targetFile.gcount();
			if (this->_targetFile.eof())
			{
				this->_fileConsumed = true;
				if (this->_bodySize == 0)
				{
					this->_body = std::string("0\r\n\r\n");
					this->_bodySize = 5;
					this->_over = true;
					return (false);
				}
			}
			else if (this->_targetFile.fail())
			{
				this->_over = true;
				return (true);
			}
			for (int i = 0; i < 15 ; i++)
			{
				std::cout << reinterpret_cast<unsigned char &>(this->_body[i]);
			}
			std::cout << std::endl;
			size << std::hex << this->_bodySize;
			size <<	"\r\n";
			std::cerr << "HEX size " << size.str() << std::endl;
			std::string	hex_size = size.str();
			std::size_t	hex_length = size.str().size();
			std::size_t	cpas = 0;
			for (std::size_t j = this->_bodySize; j > 0; j--)
			{
				this->_body[j - 1 + hex_length] = this->_body[j - 1];
			}
			for (std::string::iterator st = hex_size.begin(); st != hex_size.end(); st++, cpas++)
			{
				this->_body[cpas] = *st;
			}
			for (int i = 0; i < 15 ; i++)
			{
				std::cout << this->_body[i];
			}
			this->_bodySize += hex_length;
			std::cout << std::endl;
			this->_body[this->_bodySize] = '\r';
			this->_body[this->_bodySize + 1] = '\n';
			this->_bodySize +=  2;
			std::cerr << "Chunk size " << this->_bodySize << std::endl;
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
