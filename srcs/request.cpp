/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tnaton <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/27 17:32:13 by tnaton            #+#    #+#             */
/*   Updated: 2022/09/27 20:12:44 by tnaton           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request.hpp"

int Request::checkType(std::string & type) {
	int i = 0;

	if (type.find(" ") == std::string::npos) {
		return (1);
	} if (!type.find("GET") || !type.find("POST") || !type.find("DELETE")) {
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
		}
		if (type.c_str()[i] != '\0' || !_type.c_str() || !_file.c_str())
			return (1);
		return (0);
	}
	return (1);
}

int Request::parseChunk(std::string & chunk) {
	std::string line;

		if (_type.size()) {
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
		}
		_buff += chunk;
		if (_buff.find("\r\n\r\n") != std::string::npos) {
			//parse this
			return (1);
		}
        return (0);
}
