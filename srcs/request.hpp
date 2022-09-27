#pragma once

#include "library.hpp"

class Request{

	public:

		Request();
		~Request(){};

		int parseChunk(std::string &);
		int checkType(std::string &);
		std::string getType(void) const;
		std::string getVersion(void) const;
		std::string getFile(void) const;
		std::string getBody(void) const;
		std::map<std::string, std::list<std::string> > getHeaders(void) const;
	private:

		std::string _type;
		std::string _version;
		std::string _file;
		std::string _body;
		std::string _buff;
		std::map<std::string, std::list<std::string> > _headers;
};
