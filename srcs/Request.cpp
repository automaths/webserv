/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tnaton <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/27 17:32:13 by tnaton            #+#    #+#             */
/*   Updated: 2022/09/28 14:30:56 by tnaton           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(void): _type(""), _version("HTTP/1.0"), _file(""), _body(""), _buff(""), _headers() {
}

int Request::checkType(std::string & type) {
	int i = 0;

	if (type.find(" ") == std::string::npos) {
		return (1);
	} if (!type.find(GET) || !type.find(POST) || !type.find(DELETE)) {
		i = type.find_first_of(" ");
		_type = type.substr(0, i);
		i = type.find_first_not_of(" ", i);
		_file = type.substr(i, type.find(" ", i));
		i = type.find_first_of(" ", i);
		if (type.c_str()[i] != '\0') {
			_version = type.substr(i, type.find(" ", i));
			if (_version.size() < 6) {
				return (1);
			} if (_version.find("HTTP/")) {
				return (1);
			}
			std::string tmp = _version.substr(5, _version.size());
			if (static_cast<std::string>("123456789").find(tmp[0]) == std::string::npos)
				return (1);
			if (tmp[1] && tmp[1] == '1' && !tmp[2])
				return (1);
			if (tmp.find_first_not_of("0123456789.") != std::string::npos)
				return (1);
		}
		if (type.c_str()[i] != '\0' || !_type.c_str() || !_file.c_str())
			return (1);
		return (0);
	}
	return (1);
}

int Request::parseChunk(std::string & chunk) {
	std::string line;

		if (!_type.size()) {
			do {
				line = chunk.substr(0, chunk.find("\r\n"));
				chunk = chunk.substr(0, chunk.find("\r\n"));
				if (line != "") {
					if (checkType(_type)) {
						return (-1);
					}
				}
			}
			while (chunk.size());
		} else {
			std::string key;
			std::string val;

			do {
				line = chunk.substr(0, chunk.find("\r\n"));
				chunk = chunk.substr(0, chunk.find("\r\n"));
				if (line == "" && !chunk.find("\r\n")) {
					if (_type == POST) {
						_body = chunk;
					}
					return (1);
				}
				key = line.substr(0, (line.find(":") == std::string::npos) ? line.size() : line.find(":"));
				val = line.substr(key.size(), line.size());
				if (key.find(" ") != std::string::npos) {
					return (-1);
				}
				if (_headers.find(key) == _headers.end()) {
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
