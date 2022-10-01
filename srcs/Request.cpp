/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tnaton <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/27 17:32:13 by tnaton            #+#    #+#             */
/*   Updated: 2022/10/01 17:45:39 by tnaton           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#define NPOS std::string::npos
#define NOT_OLD _version!="HTTP/1.0"
#define NOT_NEW _version!="HTTP/1.1"

Request::Request(void): _type(""), _version("HTTP/1.0"), _file(""), _body(""), _buff(""), _headers() {
}

std::string tolower(std::string & str) {
	std::string ret;
	for (size_t i = 0; i < str.size(); i++) {
		ret.push_back(std::tolower(str[i]));
	}
	return (ret);
}

int Request::checkType(std::string & type) {
	unsigned long i = 0;

	if (type.find(" ") == NPOS) {
		return (1);
	}
	if (!type.find(GET) || !type.find(POST) || !type.find(DELETE)) {
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
		if (!_type.size() || !_file.size())
			return (1);
		return (0);
	}
	return (1);
}

void Request::parseHeaders(void) {
	std::map<std::string, std::list<std::string> >::iterator map = _headers.begin();

	if (NOT_OLD) {
		std::string & tmp = _headers["host"].front();
		tmp = tmp.substr(0, tmp.find(":"));
	}
	while (map != _headers.end()) {
		std::cout << "Key :" << map->first << std::endl;
		std::list<std::string>::iterator val = map->second.begin();
		while (val != map->second.end()) {
			if (val->find(",") != NPOS) {
				std::string tmp = val->substr(val->find(",") + 1);
				map->second.push_back(tmp.substr(tmp.find_first_not_of(" ,"), tmp.find_last_not_of(" ,") + 1));
				*val = val->erase(val->find(","));
				*val = val->substr(val->find_first_not_of(" ,"), val->find_last_not_of(" ,") + 1);
			}
			std::cout << "Val :" << *val << std::endl;
			val++;
		}
		map++;
	}
}

int Request::parseChunk(std::string & chunk) {
	std::string line;

	std::cerr << "Chunk in parsing : " << chunk << std::endl;
		if (!_type.size()) {
			do {
				line = chunk.substr(0, chunk.find("\r\n"));
				chunk.erase(0, (line.length() + 2));
				if (line != "") {
					if (checkType(line)) {
						return (400);
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
				if (_type == POST) {
					_body = chunk;
				}
				if (_headers.find("host") == _headers.end() && NOT_OLD)
					return (400);
				try {
					return (parseHeaders(), 200);
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
