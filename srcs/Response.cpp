/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsartral <nsartral@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/29 12:29:34 by bdetune           #+#    #+#             */
/*   Updated: 2022/10/19 14:38:29 by nsartral         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "cgi.hpp"
#include <ext/stdio_filebuf.h>
#include <fstream>
#include <istream>
#include <iostream>
#include <unistd.h>

Response::Response(void): _env(), _header(""), _headerSize(0), _body(""), _bodySize(0), _targetFile(), _targetFilePath(""), _headerSent(false), _chunked(false), _over(false), _fileConsumed(false), _close(false), _req(NULL), _targetServer(NULL), _targetLocation(NULL), _responseType(0), _cgi_file(""), _path_info(""), _is_cgi(false), _cgi_fd(-1), _cgi_input(-1), _root(""), _extension("")
{
	return ;
}

void	Response::closeCgiFd(void)
{
	if (this->_cgi_fd != -1)
	{
		close(this->_cgi_fd);
		this->_cgi_fd = -1;
	}
}

Response::Response(Request & req, std::vector<ServerScope> & matches, int error): _env(), _header(""), _headerSize(0), _body(""), _bodySize(0), _targetFile(), _targetFilePath(""), _headerSent(false), _chunked(false), _over(false), _fileConsumed(false), _close(false), _req(&req), _targetServer(NULL), _targetLocation(NULL), _responseType(0), _cgi_file(""), _path_info(""), _is_cgi(false), _cgi_fd(-1), _cgi_input(-1), _root(""), _extension("")
{
	std::map<std::string, std::list<std::string> >				headerMap = req.getHeaders();
	std::map<std::string, std::list<std::string> >::iterator	host;
	if (error)
		errorResponse(error);
	else
	{
		host = headerMap.find(std::string("host"));
		if (host != headerMap.end())
			this->getServer(headerMap[std::string("host")].front(), matches);
		else
			this->_targetServer = &(matches.front());
	}
	return ;
}

bool	Response::findLocation(std::vector<LocationScope> & locations, std::string uri)
{
	std::string					path;
	std::size_t					max_depth = 0;
	std::size_t					depth;
	std::string::size_type		index = 0;

	if (!locations.size())
		return (false);
	for (std::vector<LocationScope>::iterator st = locations.begin(); st != locations.end(); st++)
	{
		depth = 0;
		path = st->getMainPath();
		if (uri.find(path) == 0)
		{
			while ((index = path.find("/")) != std::string::npos)
			{
				path.erase(index, 1);
				depth += 1;
			}
		}
		if (depth > max_depth)
		{
			this->_targetLocation = &(*st);
			max_depth = depth;
		}
		else if (depth && depth == max_depth)
		{
			if (st->getMainPath().size() > this->_targetLocation->getMainPath().size())
				this->_targetLocation = &(*st);
		}
	}
	if (max_depth)
	{
		if (this->_targetLocation->getLocations().size())
			this->findLocation(this->_targetLocation->getLocations(), uri);
		return (true);
	}
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
	return (true);
}

bool	Response::foundDirectoryIndex(std::vector<std::string> indexes, std::string path)
{
	std::string	tmpIndex;
	struct stat	buf;

	if (path.size() == 0)
		path = "/";
	else if (path[path.size() - 1] != '/')
		path += "/";
	for (std::vector<std::string>::iterator st = indexes.begin(); st != indexes.end(); st++)
	{
		tmpIndex = path + *st;
		if (access(tmpIndex.data(), F_OK | R_OK) == 0 && stat(tmpIndex.data(), &buf) != -1 && !S_ISDIR(buf.st_mode) && S_ISREG(buf.st_mode))
		{
			this->_targetFilePath = tmpIndex;
			return (true);
		}
	}
	return (false);
}

#include <fstream>

int Response::execCgi(std::string exec)
{
	_env.push_back("SERVER_SOFTWARE=Webserv/1.0");
	std::vector<std::string> server_name = _targetServer->getServerName();
	std::string server_name_env;
	server_name_env = "SERVER_NAME=";
	for (std::vector<std::string>::iterator it = server_name.begin(); it != server_name.end(); ++it)
	{
		server_name_env += *it;
		if (++it != server_name.end())
			server_name_env += ":";
		--it;
	}
	_env.push_back(server_name_env);
	_env.push_back("SERVER_PROTOCOL=" + _req->getVersion());
	_env.push_back("SERVER_PORT=" + _targetServer->getPort());
	char buff[100];
	std::string cwd = "DOCUMENT_ROOT=";
	cwd += getcwd(buff, 100);
	_env.push_back(cwd);
	_env.push_back("REQUEST_METHOD=" + _req->getType());
	_env.push_back("SCRIPT_FILENAME=" + _cgi_file);
	_env.push_back("SCRIPT_NAME=" + _cgi_file);
	_env.push_back("QUERY_STRING=" + _req->getQuery());
    _env.push_back("PATH_INFO=" + _path_info);
	_env.push_back("REQUEST_URI=" + _targetFilePath);
    _env.push_back("REDIRECT_STATUS=1");
	if (_req->getHeaders().find(std::string("content-length")) != _req->getHeaders().end())
		_env.push_back(std::string("CONTENT_LENGTH=") + _req->getHeaders()[std::string("content-length")].front());
	if (_req->getHeaders().find(std::string("content-type")) != _req->getHeaders().end())
	{
		std::cerr << "Has content-type: " << _req->getHeaders()[std::string("content-type")].front() << std::endl;
		_env.push_back(std::string("CONTENT_TYPE=") + _req->getHeaders()[std::string("content-type")].front());
	}
	std::map<std::string, std::list<std::string> > map = _req->getHeaders();
	std::map<std::string, std::list<std::string> >::iterator tmp = map.begin();
	std::string	val;
	std::string temp;
	while (tmp != map.end()) {
		val = "";
		std::list<std::string>::iterator i = (*tmp).second.begin(); 
		while (i != (*tmp).second.end()) {
			val += *i;
			i++;
			if (i != (*tmp).second.end())
				val += ",";
		}
		temp.clear();
		for (unsigned int i = 0; i < (*tmp).first.size(); i++) {
			temp.push_back(std::toupper((*tmp).first[i]));
		}
		_env.push_back("HTTP_" + temp + "=" + val);
		tmp++;
	}
	std::cout << "THE ENVIRONMENT OF THE CGI" << std::endl;
	for (std::vector<std::string>::iterator it = _env.begin(); it != _env.end(); ++it)
		std::cout << "|" << *it << "|" << std::endl;
	if (this->_req->getIsBody())
	{
		this->_cgi_input = open(this->_req->getBody().data(), O_RDONLY);
		std::cout << _req->getBody() << std::endl;
		if (this->_cgi_input < 0)
		{
			this->errorResponse(500);
			return (-1);
		}
	}
	try
	{
    	Cgi test(_cgi_file, exec, _env, _cgi_input);
		this->_cgi_input = -1;
		return test.getResult();
	}
	catch (std::exception const & src)
	{
		this->_cgi_input = -1;
		this->errorResponse(500);
	}
	return (-1);
}

bool Response::internalRedirect(std::string redirect)
{
	std::vector<std::string>	indexes;
	std::string					fullPath;

	_targetLocation = NULL;
	std::cout << "REDIRECT" << redirect << std::endl;
	while (redirect.find_first_of("\t\n\v\r\f ") != std::string::npos)
		redirect.erase(redirect.find_first_of("\t\v\n\r\f ", 1));
	_req->parseUri(redirect);
	std::cout << "GETFILE" << _req->getFile() << std::endl;
	findLocation(_targetServer->getLocations(), _req->getFile());
	if ((_targetLocation && !allowedMethod(_targetLocation->getAllowMethod(), "GET")) || (!_targetLocation && !allowedMethod(_targetServer->getAllowMethod(), "GET")))
		return (false);
	fullPath = _targetLocation ? _targetLocation->getRoot() : _targetServer->getRoot();
	if (_targetLocation)
	{
		std::string	partial_root = _req->getFile();
		partial_root.erase(0, _targetLocation->getMainPath().size());
		fullPath += partial_root;
	}
	else
		fullPath += _req->getFile();
	_targetFilePath = fullPath;

	// while (fullPath.find_first_of("\t\n\v\r\f ") != std::string::npos)
	// 	fullPath.erase(fullPath.find_first_of("\t\v\n\r\f ", 1));

	if (access(fullPath.data(), F_OK) == 0)
	{
		std::cout << "THE ACCESS WORKS" << std::endl;
		return (true);
	}
	std::cout << "DEFEAT IN THE ACCESS" << std::endl;
	return (false);
}

bool Response::cgiResponse(int fd)
{
	int size;
	bool no_send = false;
	char buffer[1048576];
	memset(buffer, 0, 1048576);
	size = read(fd, &buffer, 1048576);
	if (size == 0)
	{
		_over = true;
		_fileConsumed = true;
		_body = std::string("0\r\n\r\n");
		_bodySize = 5;
		return false;
	}
	if (_headerSent)
	{
		std::stringstream hexsize;
		hexsize << std::hex << size << "\r\n";
		_body = hexsize.str() + buffer + "\r\n";
		_bodySize = _body.size();
	}
	else
	{
		_body = buffer;
		std::string cgiHeader = _body.substr(0, _body.find("\r\n\r\n") + 2);
		_body.erase(0, _body.find("\r\n\r\n") + 4);
		std::stringstream hexsize;
		hexsize << std::hex << _body.size();
		_body = hexsize.str() + "\r\n" + _body + "\r\n";
		_bodySize = _body.size();
		std::stringstream header;
		if (cgiHeader.find("Location:") != std::string::npos)
		{
			std::string str = cgiHeader.substr(cgiHeader.find("Location:") + 9, cgiHeader.find("\r\n", cgiHeader.find("Location:")) - 9);
			std::string redirect = str.substr(str.find_first_not_of("\t\v\n\r\f "), str.find("\r\n", str.find_first_not_of("\t\v\n\r\f ")));
			if (internalRedirect(redirect))
			{
				std::cout << "This is an internal redirect" << std::endl;
				_targetFilePath = _req->getFile();
				std::string type = "GET";
				_req->setType(type);
				return true;
			}
			no_send = 1;
		}
		if (cgiHeader.find("Status:") != std::string::npos)
		{
			header << "HTTP/1.1 " << cgiHeader.substr(cgiHeader.find("Status:") + 7, cgiHeader.find("\r\n", cgiHeader.find("Status:")) - 7) << "\r\n";
			std::string str = cgiHeader.substr(cgiHeader.find("Status:") + 7, cgiHeader.find("\r\n", cgiHeader.find("Status:")) - 7);
			str.erase(0, str.find_first_of("0123456789"));
			if (atoi(str.substr(0, str.find_first_not_of("0123456789")).c_str()) > 399)
				no_send = true;
			cgiHeader.erase(cgiHeader.find("Status:"), cgiHeader.find("\r\n", cgiHeader.find("Status:")) + 2);
		}
		else
			header << "HTTP/1.1 200 "<< DEFAULT200STATUS << "\r\n";
		header << setBaseHeader();
		header << cgiHeader;
		if (cgiHeader.find("Content-type:") == std::string::npos && _targetFilePath.find_last_of(".") != std::string::npos)
			header << "Content-type: " << MimeTypes().convert( _targetFilePath.substr(_targetFilePath.find_last_of("."))) << "\r\n";
		if (cgiHeader.find("Content-size:") != std::string::npos)
			cgiHeader.erase(cgiHeader.find("Content-size:"), cgiHeader.find("\r\n", cgiHeader.find("Content-size:")) + 2);
		if (no_send)
		{
			header << "Connection: close\r\n";
			_close = true;
		}
		else
			header << "Connection: keep-alive\r\n";
		if (!no_send)
			header << "Transfer-Encoding: chunked\r\n";
		header << "\r\n";
		_header = header.str();
		_headerSize = _header.size();
		if (no_send)
		{
			_body.clear();
			_bodySize = 0;
		}
	}
	memset(buffer, 0, 1048576);
	return false;
}

std::string	Response::createFileResponse(void)
{
	std::string			extension = "";
	std::stringstream	size;

	this->_responseType = 2;
	if (this->_targetFilePath.find_last_of(".") != std::string::npos)
		extension = this->_targetFilePath.substr(this->_targetFilePath.find_last_of("."));
	extension = MimeTypes().convert(extension);
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
		this->_fileConsumed = false;
		for (int i = 0; i < 15 ; i++)
		{
			std::cout << this->_body[i];
		}
		std::cout << std::endl;
		size << std::hex << this->_bodySize;
		size <<	"\r\n";
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

bool	Response::serverSet(void)
{
	return (this->_targetServer == NULL ? false : true);
}

bool	Response::canPUT(std::string fullPath)
{
	std::string::size_type	index = 0;
	std::string::size_type	current = 0;
	bool					ret = true;

	while (((index = fullPath.find('/', current)) != std::string::npos))
	{
		if (access(fullPath.substr(0, index).data(), F_OK) == 0)
		{
			if (access(fullPath.substr(0, index).data(), W_OK) != 0)
				ret = false;
			else
				ret = true;
		}
		else
			break;
		current = index + 1;
		if (current == fullPath.size())
			break ;
	}
	return (ret);
}

std::string &	Response::getTargetFile(void)
{
	return (this->_targetFilePath);
}

bool	Response::precheck(Request & req)
{
	std::vector<std::string>	indexes;
	std::string					fullPath;
	struct stat					buf;

	if (!this->_req)
		this->_req = &req;
	if (this->_responseType == 1)
		return (false) ;
	this->findLocation(this->_targetServer->getLocations(), req.getFile());
	if ((this->_targetLocation && !allowedMethod(this->_targetLocation->getAllowMethod(), req.getType())) || (!this->_targetLocation && !allowedMethod(this->_targetServer->getAllowMethod(), req.getType())))
		return (false);
	fullPath = this->_targetLocation ? this->_targetLocation->getRoot() : this->_targetServer->getRoot();
	if (this->_targetLocation)
	{
		std::string	partial_root = req.getFile();
		partial_root.erase(0, this->_targetLocation->getMainPath().size());
		fullPath += partial_root;
	}
	else
		fullPath += req.getFile();
	while (fullPath.find_first_of("\t\n\v\r\f ") != std::string::npos)
		fullPath.erase(fullPath.find_first_of("\t\v\n\r\f ", 1));
	this->_targetFilePath = fullPath;
//	std::cerr << "Fully qualified path: ***" << fullPath << "***" << std::endl;
	if (req.getType() == std::string("PUT"))
		return (true);
	if (!pathIsValid(this->_targetFilePath, &buf))
	{
		_cgi_file.clear();
		_path_info.clear();
		_is_cgi = false;
		_cgi_fd = -1;
		_cgi_input = -1;
		if (isCgiPath())
			return (true);
		else
		{
			this->errorResponse(404);
			return (false);
		}
	}
	if (S_ISDIR(buf.st_mode))
	{
		indexes = this->_targetLocation ? this->_targetLocation->getIndex() : this->_targetServer->getIndex();
		if (req.getType() == "DELETE")
			return (true);
		if (foundDirectoryIndex(indexes, this->_targetFilePath))
		{
			this->_responseType = 2;
			return (true);
		}
		else if (req.getType() == std::string("GET") && access(this->_targetFilePath.data(), R_OK) == 0)
		{
			this->_responseType = 3;
			return (true);
		}
		else
		{
			this->errorResponse(404);
			return (false);
		}
	}
	else if (S_ISREG(buf.st_mode))
	{
		if (access(fullPath.data(), F_OK | R_OK) != 0)
		{
			this->errorResponse(404);
			return (false);
		}
		return (true);
	}
	this->errorResponse(404);
	return (false);
}

ServerScope*	Response::getServerScope(void)
{
	return (this->_targetServer);
}

void	Response::setServerScope(ServerScope * target)
{
	this->_targetServer = target;
}

bool	Response::isCgiPath()
{
	if (_is_cgi == true)
		return true;
	_cgi_file.clear();
	_path_info.clear();
	if (_targetLocation)
		_root = _targetLocation->getRoot();
	else
		_root = _targetServer->getRoot();
	std::string uri = _targetFilePath;
    std::string tmp;
	std::string copy = uri;
    while (copy.find('/') != std::string::npos)
    {
        tmp = _cgi_file;
        tmp += copy.substr(0, copy.find('/'));
        if (access(tmp.data(), F_OK) == -1 && tmp.compare("/") != 0 && tmp.compare("./") != 0 && tmp.size() > 0)
        {
            std::cout << "cant access " << tmp << std::endl;
            break;
        }
        _cgi_file += copy.substr(0, copy.find('/') + 1);
        copy.erase(0, copy.find('/'));
		if (copy.size() > 0)
			copy.erase(0, 1);
    }
    if (_cgi_file.size() != uri.size())
    {
        tmp = _cgi_file;
        tmp += copy;
        if (access(tmp.data(), F_OK) != -1)
            _cgi_file += copy;
		else
		{
			if (_cgi_file.size() > 0)
				_cgi_file.erase(_cgi_file.size() - 1, 1);
		}
    }
	_path_info = uri.substr(_cgi_file.size(), uri.size() - _cgi_file.size());
	std::map<std::string, std::string> cgi = _targetServer->getCgi(); 
	if (_cgi_file.find_last_of(".") != std::string::npos)
		_extension = _cgi_file.substr(_cgi_file.find_last_of("."));
	for (std::map<std::string, std::string>::iterator it = cgi.begin(); it != cgi.end(); ++it)
	{
		if (!_extension.compare(it->first))
		{
			_is_cgi = 1;
			return (true);
		}
	}
	return (false);
}

void	Response::makeResponse(Request & req)
{
	struct stat			buf;
	std::string			extension;
	int					ret;
	
	this->_close = !req.isKeepAlive();
	if (req.getType() == std::string("PUT"))
	{
		if ((ret = req.moveBody(this->_targetFilePath)))
			this->errorResponse(ret);
	}
	else if (req.getType() == std::string("DELETE"))
	{
		ret = req.del(this->_targetFilePath);
		std::cout << "Return form delete: " << ret << std::endl;
		this->errorResponse(ret);
	}
	else if (this->_responseType == 3)
	{
		try
		{
			ListDirectory listing(this->_targetFilePath, req.getFile());
			_body = listing.listing();
			_bodySize = _body.size();
	
			std::cout << "the full path is: " << this->_targetFilePath << std::endl;
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
	}
	else if (isCgiPath())
	{
		std::map<std::string, std::string> cgi = _targetServer->getCgi(); 
		std::string extension;
		if (_cgi_file.find_last_of(".") != std::string::npos)
			extension = _cgi_file.substr(_cgi_file.find_last_of("."));
		for (std::map<std::string, std::string>::iterator it = cgi.begin(); it != cgi.end(); ++it)
		{
			if (!extension.compare(it->first))
			{
				std::cout << "extension " << extension << " match the config extension " << it->first << " associated to path " << it->second << std::endl;
				std::cout << "Execution of the cgi" << std::endl;
				try
				{
					_cgi_fd = execCgi(it->second);
				}
				catch (std::exception const & e)
				{
					_is_cgi = 0;
					this->errorResponse(500);
					return ;
				}
				std::cout << "the cgi_fd is: " << _cgi_fd << std::endl;
				return ;
			}
		}
	}
	else
	{
		this->_targetFile.open(this->_targetFilePath.data(), std::ifstream::binary);
		if (!this->_targetFile.fail())
		{
			if (stat(this->_targetFilePath.data(), &buf) == -1)
			{
				this->_targetFile.close();
				this->errorResponse(500);
				return ;
			}
			if (buf.st_size > 1048576)
			{
				this->_chunked = true;
				this->_bodySize = 1048586;
			}
			else
				this->_bodySize = buf.st_size;
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
			this->errorResponse(500);
	}
}

Response::Response(Response const & src): _env(src._env), _header(src._header), _headerSize(src._headerSize), _body(src._body), _bodySize(src._bodySize), _headerSent(src._headerSent), _chunked(src._chunked), _over(src._over), _fileConsumed(src._fileConsumed), _close(src._close), _req(src._req), _targetServer(src._targetServer), _targetLocation(src._targetLocation), _responseType(src._responseType), _is_cgi(src._is_cgi), _cgi_fd(src._cgi_fd), _cgi_input(-1), _root(""), _extension("")
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
	this->closeCgiFd();
	this->_env = rhs._env;
	this->_header = rhs._header;
	this->_headerSize = rhs._headerSize;
	this->_body = rhs._body;
	this->_bodySize = rhs._bodySize;
	this->_targetFilePath = rhs._targetFilePath;
	this->_headerSent = rhs._headerSent;
	this->_chunked = rhs._chunked;
	this->_over = rhs._over;
	this->_fileConsumed = rhs._fileConsumed;
	this->_close = rhs._close;
	this->_req = rhs._req;
	this->_targetServer = rhs._targetServer;
	this->_targetLocation = rhs._targetLocation;
	this->_responseType = rhs._responseType;
	this->_cgi_file = rhs._cgi_file;
	this->_is_cgi = rhs._is_cgi;
	this->_cgi_fd = rhs._cgi_fd;
	this->_path_info = rhs._path_info;
	this->_cgi_input = rhs._cgi_input;
	this->_root = rhs._root;
	this->_extension = rhs._extension;
	return (*this);
}

void	Response::errorResponse(int error)
{
	std::stringstream	header;
	std::stringstream	status;
	std::stringstream	code;
	std::string			body;
	std::string			mime;

	this->_responseType = 1;
	this->_close = true;
	code << error;
	this->_targetFilePath.clear();
	if (this->_targetLocation && this->_targetLocation->getDefaultErrorPage().find(code.str()) != this->_targetLocation->getDefaultErrorPage().end())
		this->_targetFilePath = (this->_targetLocation->getDefaultErrorPage())[code.str()];
	else if (!this->_targetLocation && this->_targetServer && this->_targetServer->getDefaultErrorPage().find(code.str()) != this->_targetServer->getDefaultErrorPage().end())
		this->_targetFilePath = (this->_targetServer->getDefaultErrorPage())[code.str()];
	if (this->_targetFilePath.size())
	{
		if (this->openFile(this->_targetFilePath))
		{
			mime = this->createFileResponse();
			if (_is_cgi)
				return ;
			if (this->_responseType == 2)
			{
				this->createFileErrorHeader(error, mime);
				return ;
			}
			else
			{
				this->_targetFile.close();
				if (error != 500)
				{
					this->errorResponse(500);
					return ;
				}	
			}
		}
		// else
		// {
		// 	if (error != 404)
		// 	{
		// 		this->errorResponse(404);
		// 		return ;
		// 	}
		// }
	}
	switch (error)
	{
		case 200:
			status << " 200 " << DEFAULT200STATUS;
			body = DEFAULT200BODY;
			break ;
		case 201:
			status << " 201 " << DEFAULT201STATUS;
			body = DEFAULT201BODY;
			break ;
		case 206:
			status << " 206 " << DEFAULT201STATUS;
			body = DEFAULT206BODY;
			break ;
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
		case 507:
			status << " 507 " << DEFAULT507STATUS;
			body = DEFAULT507BODY;
			break ;
		default:
			status << " 500 " << DEFAULT500STATUS;
			body = DEFAULT500BODY;
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
		case 200:
			status << " 200 " << DEFAULT200STATUS;
			break ;
		case 201:
			status << " 201 " << DEFAULT201STATUS;
			break ;
		case 206:
			status << " 206 " << DEFAULT201STATUS;
			break ;
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
			status << " 500 " << DEFAULT500STATUS;
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
		else if (this->_responseType == 2 && this->_chunked && this->_fileConsumed && this->_over)
		{
			return (true);
		}
		else if (this->_responseType == 2 && this->_chunked && this->_fileConsumed && !this->_over)
		{
			this->_body = std::string("0\r\n\r\n");
			this->_bodySize = 5;
			this->_over = true;
			return (false);
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
				return (true);
			}
		}
	}
	this->_targetServer = &(matches.front());
	return (false);
}
