#pragma once
#define GET "GET"
#define POST "POST"
#define DELETE "DELETE"
#include <string>
#include <map>
#include <list>
#include <utility>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <unistd.h>

class Request{

	public:

		Request();
		~Request(){};

		int			parseHeaders(void);
		int			parseChunk(std::string &);
		int			checkType(std::string &);
		std::string	getType(void) const;
		std::string	getVersion(void) const;
		std::string	getFile(void) const;
		std::string	getBody(void) const;
		std::map<std::string, std::list<std::string> > getHeaders(void) const;
	private:

		std::string		_type;
		std::string		_version;
		std::string		_file;
		std::string		_body;
		std::string		_buff;
		std::map<std::string, std::list<std::string> > _headers;
		bool			_isbody;
		unsigned int	_bodysize;
};
