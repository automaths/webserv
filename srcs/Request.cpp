/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsartral <nsartral@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/27 17:32:13 by tnaton            #+#    #+#             */
/*   Updated: 2022/10/17 15:36:10 by tnaton           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#define NPOS std::string::npos
#define NOT_OLD _version!="HTTP/1.0"
#define NOT_NEW _version!="HTTP/1.1"

Request::Request(void): _type(""), _version(""), _file(""), _body(""), _headers(), _isbody(false), _bodysize(-1), _putfile(), _tmpfile(), _query(""), _keepalive(true) {
}

Request::Request(const Request & other): _type(other._type), _version(other._version), _file(other._file), _body(other._body), _headers(other._headers), _isbody(other._isbody), _bodysize(other._bodysize), _putfile(), _tmpfile(), _query(other._query), _keepalive(other._keepalive) {
}

Request::~Request(void) {
	if (_putfile.is_open()) {
		unlink(_body.data());
	}
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
	_query = other._query;
	_keepalive = other._keepalive;
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
		ref = tmp + 1;
		if (*tmp == ".." || *tmp == "/.." || *tmp == "../" || *tmp == "/../") {
			v.erase(tmp.base());
			count++;
		} else if (count) {
			v.erase(tmp.base());
			count--;
		}
		tmp = ref;
	}
	if (count) {
		file = "/";
	} else {
		while (v.size()) {
			file += v.front();
			v.erase(v.begin());
		}
	}
	if (!file.size())
		file = "/";
	return (file);
}

std::string PercentDecoding(std::string uri) {
	std::string		ret;
	size_t			pos;
	std::map<std::string, std::string> map;

	map["%20"] = " ";
	map["%21"] = "!";
	map["%23"] = "#";
	map["%24"] = "$";
	map["%25"] = "%";
	map["%26"] = "&";
	map["%27"] = "'";
	map["%28"] = "(";
	map["%29"] = ")";
	map["%2A"] = "*";
	map["%2B"] = "+";
	map["%2C"] = ",";
	map["%2F"] = "/";
	map["%3A"] = ":";
	map["%3B"] = ";";
	map["%3D"] = "=";
	map["%3F"] = "?";
	map["%40"] = "@";
	map["%5B"] = "[";
	map["%5D"] = "]";

	while (uri.size()) {
		if ((pos = uri.find("%")) != NPOS) {
			if (map.find(uri.substr(pos, 3)) != map.end()) {
				ret += uri.substr(0, pos) + map[uri.substr(pos, 3)];
			} else {
				ret += uri.substr(0, pos + 3);
			}
			uri.erase(0, pos + 3);
		} else {
			ret += uri;
			break;
		}
	}
	return (ret);
}

void Request::parseUri(std::string file) {
	if (file.find("?") != NPOS) {
		_query = file.substr(_file.find("?") + 1, NPOS);
		file.erase(file.find("?"), NPOS);
	}
	_file = PercentDecoding(file);
}

int Request::checkType(std::string & type) {
	unsigned long i = 0;

	if (type.find(" ") == NPOS) {
		return (1);
	}
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
	parseUri(_file);
	if (_file[0] != '/')
		_file = "/" + _file;
	if (!_file.size())
		return (1);
	_file = parseFile(_file);
	return (0);
}

std::string Request::getQuery() const { return _query; }

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

int	rm(const char *fpath, const struct stat *buf, int typeflag, struct FTW *ftwbuf) {
	(void)ftwbuf;
	(void)buf;

	if (typeflag == FTW_DP) {
		if (rmdir(fpath))
			return (1);
	} else {
		if (unlink(fpath)) {
			return (1);
		}
	}
	return (0);
}

bool nuke(std::string & path) {
	if (nftw(path.data(), &rm, 1, FTW_DEPTH))
		return (false);
	return (true);
}

int del(std::string & path) {
	if (access(path.data(), F_OK))
		return (404);
	if (nuke(path))
		return (403);
	return (200);
}

int createPath(std::string & path) {
	std::string	tmp = "";
	int			i = 0;
	struct		stat buf;

	while (path != tmp && path.size() > tmp.size()) {
		if (access(tmp.data(), F_OK)) {
			mkdir(tmp.data(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
		}
		tmp += path.substr(i, path.find("/", i) + 1 - i);
		i = path.find("/", i) + 1;
	}
	if (!access(path.data(), F_OK) && !stat(path.data(), &buf) && S_ISDIR(buf.st_mode)) {
		if (nuke(path))
			return (403);
	} else if (access(path.data(), F_OK))
		return (201);
	unlink(path.data());
	return (200);
}

int Request::moveBody(std::string & path) {
	char	buff[1048576];
	int		code = 200;

	if (!_putfile.is_open()) {
		if ((code = createPath(path)) == 403)
			return (403);
		_putfile.open(path.data(), std::ios::binary | std::ios::trunc);
		_tmpfile.open(_body.data(), std::ios::binary);
	}
	_tmpfile.read(buff, 1048576);
	_putfile.write(buff, _tmpfile.gcount());
	if (_tmpfile.eof()) {
		_tmpfile.close();
		_putfile.close();
		unlink(_body.data());
		return (code);
	}
	return (0);
}

void Request::parseBody(std::string & chunk) {
	if (_body == "") {
		_body = checkopen("0");
	}
	if (!_putfile.is_open())
		_putfile.open(_body.data(), std::ios::binary);
	_putfile.write(chunk.data(), chunk.size());
	_bodysize -= chunk.size();
}

bool	Request::getIsBody() const {
	return (this->_isbody);
}

int Request::parseChunk(std::string & chunk) {
	std::string line;

	if (_isbody) {
		parseBody(chunk);
		if (_bodysize > 0) {
			return (0);
		}
		_putfile.close();
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
					} else if (_type == HEAD) {
						return (405);
					} else if (!(_type == GET || _type == POST || _type == PUT || _type == DELETE)) {
						return (501);
					} else if (_version == "") {
						_keepalive = false;
						return (200);
					} else if (NOT_NEW && NOT_OLD) {
						return (505);
					} if (_version == "HTTP/1.0") {
						_keepalive = false;
					} else {
						_keepalive = true;
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
				if (_headers.find("connection") != _headers.end()) {
					if (_headers["connection"].front() == "close") {
						_keepalive = false;
					} else if (_headers["connection"].front() == "keep-alive") {
						_keepalive = true;
					}
				} if (_headers.find("host") == _headers.end() && NOT_OLD) {
					return (400);
				} if (_type == POST || _type == PUT) {
					_isbody = true;
					if (_headers.find("content-length") != _headers.end()) {
						_bodysize = ft_atoi(_headers["content-length"].front());
					}
					parseBody(chunk);
					if (_bodysize > 0)
						return (201);
					return (200);
				} try {
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

std::vector<std::pair<int, int> > Request::getRange(int	size) {
	std::vector<std::pair<int, int> >	ret;
	std::list<std::string>				lst(_headers["range"]);
	std::string							tmp;

	while (lst.size()) {
		tmp = lst.front();
		if (tmp[0] == '-') {
			ret.push_back(std::pair<int, int>(size - std::atoi(tmp.substr(1, NPOS).data()), size));
		} else if (tmp[tmp.size()] == '-') {
			ret.push_back(std::pair<int, int>(std::atoi(tmp.substr(0, tmp.size() - 1).data()), size));
		} else {
			ret.push_back(std::pair<int, int>(std::atoi(tmp.substr(0, tmp.find("-")).data()), std::atoi(tmp.substr(tmp.find("-") + 1, NPOS).data())));
		}
		lst.pop_front();
	}
	return (ret);
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

void Request::setQuery(std::string & str) {
	_query = str;
}

void Request::setFile(std::string & str) {
	_file = str;
}

void Request::setType(std::string & str) {
	_type = str;
}

bool Request::isKeepAlive(void) const {
	return (_keepalive);
}
