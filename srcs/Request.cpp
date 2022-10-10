/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsartral <nsartral@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/27 17:32:13 by tnaton            #+#    #+#             */
/*   Updated: 2022/10/10 20:18:14 by tnaton           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#define NPOS std::string::npos
#define NOT_OLD _version!="HTTP/1.0"
#define NOT_NEW _version!="HTTP/1.1"

Request::Request(void): _type(""), _version(""), _file(""), _body(""), _buff(""), _headers(), _isbody(false), _bodysize(0), _putfile(), _tmpfile() {
}

Request::Request(const Request & other): _type(other._type), _version(other._version), _file(other._file), _body(other._body), _headers(other._headers), _isbody(other._isbody), _bodysize(other._bodysize), _putfile(), _tmpfile() {
}

Request & Request::operator=(const Request & other) {
	if (this == &other)
		return (*this);
	_type = other._type;
	_version = other._version;
	_file = other._file;
	_body = other._body;
	_headers = other._headers;
	_isbody = other._isbody;
	_bodysize = other._bodysize;
	return (*this);
}

std::string tolower(std::string str) {
	std::string ret;
	for (size_t i = 0; i < str.size(); i++) {
		ret.push_back(std::tolower(str[i]));
	}
	return (ret);
}

std::string parseFile(std::string file) {
	std::vector<std::string>					v;
	int											count = 0;
	std::vector<std::string>::reverse_iterator	ref;
	std::vector<std::string>::reverse_iterator	tmp;

	while (file.size()) {
		std::cout << "File in while : " << file << " with size : " << file.size() << std::endl;
		if (file.find("/") != NPOS) {
			v.push_back(file.substr(0, file.find("/") + 1));
			file.erase(0, file.find("/") + 1);
		} else {
			v.push_back(file);
			file.erase(0, NPOS);
		}
		count++;
		if (count == 100)
			break ;
	}
	count = 0;

	tmp = v.rbegin();
	while (tmp != v.rend()) {
		std::cout << "tmp : " << *tmp << std::endl;
		ref = tmp + 1;
		if (*tmp == ".." || *tmp == "/.." || *tmp == "../" || *tmp == "/../") {
			v.erase(tmp.base());
			count++;
		} else if (count) {
			std::cout << "To be erased : " << *tmp << std::endl;
			v.erase(tmp.base());
			std::cout << "To be erased : " << *tmp << std::endl;
			count--;
		}
		tmp = ref;
		std::cout << "count : " << count << std::endl;
	}
	if (count) {
		file = "/";
	} else {
		while (v.size()) {
			std::cout << "file in second while : " << file << std::endl;
			file += v.front();
			v.erase(v.begin());
		}
	}
	if (!file.size())
		file = "/";
	std::cout << "file : " << file << std::endl;
	return (file);
}

int Request::checkType(std::string & type) {
	unsigned long i = 0;

	if (type.find(" ") == NPOS) {
		return (1);
	}
	if (!type.find(GET) || !type.find(POST) || !type.find(DELETE) || !type.find(PUT)) {
		i = type.find_first_of(" ");
		_type = type.substr(0, i);
		i = type.find_first_not_of(" ", i);
		type.erase(0, i);
		_file = type.substr(0, type.find_first_of(" "));
		type.erase(0, _file.size());
		type.erase(0, type.find_first_not_of(" "));
		if (type.size()) {
			_version = type.substr(0, type.find(" "));
			type.erase(0, _version.size());
			if (_version.size() < 6) {
				return (1);
			} if (_version.find("HTTP/")) {
				return (1);
			}
			std::string tmp = _version.substr(5);
			if (static_cast<std::string>("123456789").find(tmp[0]) == NPOS)
				return (1);
			if (tmp == "1")
				return (1);
			if (tmp.find_first_not_of("0123456789.") != NPOS)
				return (1);
			std::string tmp2 = tmp.substr(tmp.find(".") + 1);
			if (tmp2 != tmp && (tmp2 == "" || tmp2.find_first_not_of("0123456789") != NPOS))
				return (1);
		}
		if (!_file.size())
			return (1);
		_file = parseFile(_file);
		return (0);
	}
	return (1);
}

int ft_atoi(std::string & str) {
	if (str.size() >= 10) {
		throw (std::exception());
	}
	return (atoi(str.c_str()));
}

int Request::parseHeaders(void) {
	std::map<std::string, std::list<std::string> >::iterator map = _headers.begin();

	if (_headers.find("host") != _headers.end()) {
		std::string & tmp = _headers["host"].front();
		tmp = tmp.substr(0, tmp.find(":"));
	}
	while (map != _headers.end()) {
		std::list<std::string>::iterator val = map->second.begin();
		while (val != map->second.end()) {
			if (val->find(",") != NPOS) {
				std::string tmp = val->substr(val->find(",") + 1);
				map->second.push_back(tmp.substr(tmp.find_first_not_of(" ,"), tmp.find_last_not_of(" ,") + 1));
				*val = val->erase(val->find(","));
				*val = val->substr(val->find_first_not_of(" ,"), val->find_last_not_of(" ,") + 1);
			}
			val++;
		}
		map++;
	}
	if (_headers.find("range") != _headers.end()) {
		std::string tmp = _headers["range"].front();
		if (tolower(tmp) != "bytes" && tolower(tmp.substr(0, tmp.find("="))) == "bytes" && tmp.substr(tmp.find("=") + 1).size()) {
			_headers["range"].pop_front();
			_headers["range"].push_front(tmp.substr(tmp.find("=") + 1));
			for (std::list<std::string>::iterator val = _headers["range"].begin(); val != _headers["range"].end(); val++) {
				if ((*val).find("-") == NPOS || *val == "-")
					return (416);
				std::string lhs = (*val).substr(0, (*val).find("-"));
				std::string rhs = (*val).substr((*val).find("-") + 1);
				if (lhs.find_first_not_of("0123456789") != NPOS || rhs.find_first_not_of("0123456789") != NPOS)
					return (416);
				if (lhs.size() && rhs.size()) {
					try {
						if (ft_atoi(lhs) > ft_atoi(rhs))
							return (416);
					} catch (std::exception & e) {
						return (416);
					}
				}
			}
		} else {
			_headers.erase("range");
		}
	}
	map = _headers.begin();
	while (map != _headers.end()) {
		std::cout << "Key :" << map->first << std::endl;
		std::list<std::string>::iterator val = map->second.begin();
		while (val != map->second.end()) {
			std::cout << "Val :" << *val << std::endl;
			val++;
		}
		map++;
	}
	return (200);
}

std::string newchar(std::string str)
{
	int					num;
	std::stringstream	ret;

	num = atoi(str.c_str());
	num++;
	ret << num;
	return (ret.str());
}

std::string	checkopen(std::string str)
{
	int			ret;
	std::string	tmp;

	ret = 0;
	while (!ret)
	{
		tmp = "/tmp/.nomrandomdeswebcerveaux" + str;
		ret = access(tmp.c_str(), F_OK);
		if (!ret)
			str = newchar(str);
	}
	return (tmp);
}

bool Request::moveBody(std::string & path) {
	char	buff[1048576];
	if (!_putfile.is_open()) {
		unlink(path.data());
		_putfile.open(path.data(), std::ios::binary);
		_tmpfile.open(_body.data(), std::ios::binary);
	}
	_tmpfile.read(buff, 1048576);
	_putfile.write(buff, _tmpfile.gcount());
	if (_tmpfile.eof()) {
		_tmpfile.close();
		_putfile.close();
		unlink(_body.data());
		return (true);
	}
	return (false);
}

void Request::parseBody(std::string & chunk) {
	if (_body == "") {
		_body = checkopen("0");
	}
	std::ofstream	file(_body.data(), std::ios::binary);

	file.write(chunk.data(), chunk.size());
	_bodysize -= chunk.size();
}

int Request::parseChunk(std::string & chunk) {
	std::string line;

	std::cerr << "Chunk in parsing : " << chunk << std::endl;

	if (_isbody) {
		parseBody(chunk);
		if (_bodysize)
			return (201);
		return (200);
	} else {
		if (chunk.find("\x03") != NPOS)
			return (1);
		if (!_type.size()) {
			do {
				line = chunk.substr(0, chunk.find("\r\n"));
				chunk.erase(0, (line.length() + 2));
				if (line != "") {
					if (checkType(line)) {
						return (400);
					} else if (_version == "") {
						return (200);
					} else if (NOT_NEW && NOT_OLD) {
						return (505);
					}
					break;
				}
			}
			while (chunk.size());
		}
		if (!chunk.size())
			return (0);
		std::string key;
		std::string val;

		do {
			line = chunk.substr(0, chunk.find("\r\n"));
			chunk.erase(0, (line.length() + 2));
			if (line == "") {
				if (_type == POST || _type == PUT) {
					_isbody = true;
					if (_headers.find("content-length") != _headers.end()) {
						_bodysize = ft_atoi(_headers["content-length"].front());
					}
					parseBody(chunk);
					if (_bodysize)
						return (201);
					return (200);
				}
				if (_headers.find("host") == _headers.end() && NOT_OLD)
					return (400);
				try {
					return (parseHeaders());
				} catch (std::exception & e) {
					std::cerr << "Erreur dans le parsing du header :" << e.what() << std::endl;
					return (200);
				}
			}
			key = line.substr(0, (line.find(":") == NPOS) ? line.size() : line.find(":"));
			key = tolower(key);
			(line.find(":") == NPOS) ? line.erase(0, (key.size())) : line.erase(0, (key.size() + 1));
			val = line;
			if (val.size() && val.find_first_not_of(" ,") != NPOS)
				val = line.substr(line.find_first_not_of(" ,"), line.find_last_not_of(" ,") - line.find_first_not_of(" ,") + 1);
			else
				val = "";
			if (key.find(" ") != NPOS) {
				return (400);
			} if (key == "host") {
				if (val == "" || _headers.find(key) != _headers.end()) {
					return (400);
				}
			} if (_headers.find(key) == _headers.end()) {
				_headers.insert(std::pair<std::string, std::list<std::string> >(key, std::list<std::string>(1, val)));
			} else {
				_headers[key].push_back(val);
			}
		}
		while (chunk.size());
	}
    return (0);
}

std::string Request::getType(void) const {
	return (_type);
}

std::string Request::getVersion(void) const {
	return (_version);
}

std::string Request::getFile(void) const {
	return (_file);
}

std::string Request::getBody(void) const {
	return (_body);
}

std::map<std::string, std::list<std::string> > Request::getHeaders(void) const {
	return (_headers);
}
