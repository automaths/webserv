/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tnaton <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/27 17:32:13 by tnaton            #+#    #+#             */
/*   Updated: 2022/09/28 18:08:03 by bdetune          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(void): _type(""), _version("HTTP/1.0"), _file(""), _body(""), _buff(""), _headers() {
}

int Request::checkType(std::string & type) {
	int i = 0;

	std::cout << "First line received: |" << type << "|" <<std::endl;
	if (type.find(" ") == std::string::npos) {
		return (1);
	}
	if (!type.find(GET) || !type.find(POST) || !type.find(DELETE)) {
		std::cerr << "Condition" << std::endl;
		i = type.find_first_of(" ");
		_type = type.substr(0, i);
		i = type.find_first_not_of(" ", i);
		type.erase(0, i);
		_file = type.substr(0, type.find_first_of(" "));
		std::cout << "type: |"<< _type << "|" << std::endl;
		std::cout << "file: |"<< _file << "|" << std::endl;
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
			std::cout << "version: |"<< _version << "|" << std::endl;
			std::string tmp = _version.substr(5);
			if (static_cast<std::string>("123456789").find(tmp[0]) == std::string::npos)
				return (1);
			if (tmp[1] && tmp[1] == '1' && !tmp[2])
				return (1);
			if (tmp.find_first_not_of("0123456789.") != std::string::npos)
				return (1);
			std::cout << "tmp: |"<< tmp << "|" << std::endl;
		}
		if (type.size() || !_type.size() || !_file.size())
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
				chunk.erase(0, (line.length() + 2));
				if (line != "") {
					if (checkType(line)) {
						return (-1);
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
				return (1);
			}
			key = line.substr(0, (line.find(":") == std::string::npos) ? line.size() : line.find(":"));
			(line.find(":") == std::string::npos) ? line.erase(0, (key.size() + 1)) : line.erase(0, (key.size()));
			val = line;
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
