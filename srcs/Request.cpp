/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsartral <nsartral@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/27 17:32:13 by tnaton            #+#    #+#             */
/*   Updated: 2022/10/24 18:28:16 by tnaton           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#define NPOS std::string::npos
#define NOT_OLD _version!="HTTP/1.0"
#define NOT_NEW _version!="HTTP/1.1"

extern volatile std::sig_atomic_t g_code;

Request::Request(void): _type(""), _version(""), _file(""), _body(""), _headers(), _isbody(false), _bodysize("0"), _putfile(), _tmpfile(), _query(""), _maxBodySize(""), _keepalive(true), _ischunked(false), _case(1), _bufsize("\0"), _uri(NULL), _currentsize("0") {
}

Request::Request(const Request & other): _type(other._type), _version(other._version), _file(other._file), _body(other._body), _headers(other._headers), _isbody(other._isbody), _bodysize(other._bodysize), _putfile(), _tmpfile(), _query(other._query), _maxBodySize(other._maxBodySize), _keepalive(other._keepalive), _ischunked(other._ischunked), _case(other._case), _currentsize(other._currentsize) {
	strcpy(_bufsize, other._bufsize);
	if (other._uri) {
		_uri = new char[8192];
		strcpy(_uri, other._uri);
	} else 
		_uri = NULL;
}

Request::~Request(void) {
	 if (g_code != 1 && (_putfile.is_open() || (_body != "" && !access(_body.data(), F_OK)))) {
		unlink(_body.data());
	}
	 if (_uri)
		 delete[] _uri;
}

Request & Request::operator=(const Request & other) {
	if (this == &other)
		return (*this);
	if (_body != "") {
		unlink(_body.data());
	}
	_type = other._type;
	_version = other._version;
	_file = other._file;
	_body = other._body;
	_headers = other._headers;
	_isbody = other._isbody;
	_bodysize = other._bodysize;
	_query = other._query;
	_maxBodySize = other._maxBodySize;
	_keepalive = other._keepalive;
	_ischunked = other._ischunked;
	_case = other._case;
	strcpy(_bufsize, other._bufsize);
	if (_uri)
		delete[] _uri;
	_uri = other._uri;
	_currentsize = other._currentsize;
	return (*this);
}

std::string	Request::getBodySize(void) const
{
	return (this->_bodysize);
}

void	Request::setMaxBodySize(std::string & maxBodySize)
{
	this->_maxBodySize = maxBodySize;
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

std::string Request::getQuery() const {
	return _query;
}

int ft_atoi(std::string str) {
	if (str.size() < 10 || (str.size() == 10 && strcmp("2147483647", str.data()) > 0)) {
		return (atoi(str.c_str()));
	}
	return (2147483647);
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

bool Request::nuke(std::string & path) {
	if (nftw(path.data(), &rm, 1, FTW_DEPTH))
		return (false);
	return (true);
}

int Request::del(std::string & path) {
	if (access(path.data(), F_OK))
		return (404);
	if (!nuke(path))
		return (403);
	return (200);
}

int Request::createPath(std::string & path) {
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
		if (!nuke(path))
			return (403);
	} else if (access(path.data(), F_OK))
		return (201);
	unlink(path.data());
	return (200);
}

int Request::moveBody(std::string & path, char *buff, unsigned long sizebuff) {
	int		code = 200;

	if (!_putfile.is_open()) {
		if ((code = createPath(path)) == 403)
			return (403);
		_putfile.open(path.data(), std::ios::binary | std::ios::trunc);
		_tmpfile.open(_body.data(), std::ios::binary);
	}
	_tmpfile.read(buff, sizebuff);
	_putfile.write(buff, _tmpfile.gcount());
	if (_putfile.fail()) {
		_tmpfile.close();
		_putfile.close();
		unlink(path.data());
		return (507);
	}
	if (_tmpfile.eof()) {
		_tmpfile.close();
		_putfile.close();
		unlink(_body.data());
		return (code);
	}
	return (0);
}

std::string minus(std::string _bs, unsigned long chunksize) {
	std::stringstream ret;

	for (unsigned long i = 0; i < _bs.size(); i++)
		ret << 0;
	ret << chunksize;
	std::string str = ret.str();
	for (unsigned long i = 0; i < _bs.size(); i++) {
		if (i < str.size()) {
			if (str[str.size() - 1 - i] > '9') {
				str[str.size() - 2 - i] += 1;
				str[str.size() - 1 - i] = '0';
			} if (str[str.size() - 1 - i] > _bs[_bs.size() - 1 - i]) {
				_bs[_bs.size() - 1 - i] = '9' + 1 - (str[str.size() - 1 - i] - _bs[_bs.size() - 1 - i]); 
				str[str.size() - 2 - i] += 1;
			} else {
				_bs[_bs.size() - 1 - i] = '0' + _bs[_bs.size() - 1 - i] - str[str.size() - 1 - i];
			}
		}
	}
	while (_bs[0] == '0')
		_bs = _bs.substr(1);
	if (!_bs.size())
		return ("0");
	return (_bs);
}

int ft_strlen(char *str) {
	int i = 0;

	if (!str)
		return 0;
	while (str[i])
		i++;
	return (i);
}



void Request::parseBodyChunked(std::string & chunk) {
	while (chunk.size()) {
		
		
		
		switch(_case) { 
			case(1) : { 
				
				std::stringstream	hex;
				std::stringstream	dec;
				unsigned long		tmp;
				std::string			test;

				if (chunk.find_first_not_of("0\r\n") != NPOS) {
					chunk.erase(0, chunk.find_first_not_of('0'));
				} else {
					chunk.erase(0, chunk.find_first_not_of('0'));
					chunk = "0" + chunk;
				}
				if (ft_strlen(_bufsize))
					chunk = _bufsize + chunk;
				tmp = chunk.find("\r\n");
				if (!chunk.substr(0, tmp).size()) {
					_bodysize = "Hexa";
					return ;
				}
				if (tmp == NPOS || tmp > 7) {
					if (chunk.size() > 7) {
						
						_bodysize = "Big";
						return ;
					}
					strcpy(_bufsize, chunk.data());
					return ;
				}
				hex << std::hex << 0 << chunk.substr(0, tmp);
				chunk.erase(0, tmp + 2);
				test = hex.str();
				hex >> tmp;
				dec << std::dec << tmp;
				_bodysize = dec.str();
				for (std::string::const_iterator it = test.begin(); it != test.end(); it++) {
					if (static_cast<std::string>("0123456789abcdfABCDEF").find(*it) == NPOS) {
						_bodysize = "Hexa";
						return ;
					}
				}
				_case = 2;
				if (_bodysize == "0") {
					_case = 3;
				}
				break;
			}
			case(2) : {
				
				if (chunk.size() > static_cast<unsigned long>(ft_atoi(_bodysize))) {
					_putfile.write(chunk.data(), ft_atoi(_bodysize));
					chunk.erase(0, ft_atoi(_bodysize));
					if (isSuperiorStringNumbers(_currentsize = addStringNumbers(_currentsize, _bodysize), _maxBodySize) && _maxBodySize != "") {
						_bodysize = "Big";
						return ;
					}
					_bodysize = "chunked";
					_case = 3;
				} else {
					_putfile.write(chunk.data(), chunk.size());
					_bodysize = minus(_bodysize, chunk.size());
					if (isSuperiorStringNumbers(_currentsize = addStringNumbers(_currentsize, numberToString(chunk.size())), _maxBodySize) && _maxBodySize != "") {
						_bodysize = "Big";
						return ;
					}
					if (_bodysize == "0") {
						_bodysize = "chunked";
						_case = 3;
					}
					chunk = "";
				}
				break;
			}
			case(3) : {
				
				if (chunk.size() == 1 && chunk == "\r") {
					
					_bodysize = "wtf";
					return ;
				}
				if (_bodysize == "wtf") {
					
					_bodysize = "0";
					chunk = "\r" + chunk;
					
					for (std::string::iterator it = chunk.begin(); it != chunk.end(); it++) {
						printf("%d\n", *it);
					}
				}
				if (chunk.size() < 2 || (chunk.substr(0, 2) != "\r\n")) {
					_bodysize = "Hexa";
					return ;
				}
				_case = 1;
				chunk.erase(0, 2);
				if (!chunk.size())
					return ;
				break;
			}
		}
		
		
	}
}

void Request::parseBody(std::string & chunk) {
	if (_body == "") {
		_body = checkopen("0");
	}
	if (!_putfile.is_open())
		_putfile.open(_body.data(), std::ios::binary);
	if (!_ischunked) {
		if (chunk.size() > static_cast<unsigned long>(ft_atoi(_bodysize))) {
			_putfile.write(chunk.data(), ft_atoi(_bodysize));
			_bodysize = "0";
		} else {
			_putfile.write(chunk.data(), chunk.size());
			_bodysize = minus(_bodysize, chunk.size());
		}
		
	} else {
		parseBodyChunked(chunk);
	}
	if (_putfile.bad()) {
		_putfile.close();
		unlink(_body.data());
		_bodysize = "Error";
	}
}

bool	Request::getIsBody() const {
	return (this->_isbody);
}

int Request::parseChunk(std::string & chunk) {
	std::string line;

	
	if (_isbody) {
		parseBody(chunk);
		if (_bodysize == "Error") {
			return (507);
		} else if (_bodysize == "Hexa") {
			return (400);
		} else if (_bodysize == "Big") {
			return (413);
		} else if (_bodysize != "0" || _case == 3) {
			return (0);
		}
		_putfile.close();
		return (200);
	} else {
		if (!_type.size()) {
			do {
				if (_uri) {
					chunk =  _uri + chunk;
					delete[] _uri;
					_uri = NULL;
				}
				if (chunk.find("\r\n") == NPOS) {
					if (ft_strlen(_uri) + chunk.size() > 8192) {
						return (414);
					} else { 
						if (!_uri) {
							_uri = new char[8192];
						}
						strcpy(_uri, chunk.data());
						return (0);
					}
				} else if (ft_strlen(_uri) + chunk.find("\r\n") > 8192) {
					return (414);
				}
				line = chunk.substr(0, chunk.find("\r\n"));
				chunk.erase(0, (line.length() + 2));
				if (line != "") {
					if (checkType(line)) {
						return (400);
					} else if (!(_type == GET || _type == POST || _type == PUT || _type == DELETE)) {
						return (501);
					} else if (_version == "") {
						
						_keepalive = false;
						_type = GET;
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
			if (_uri) {
				
				chunk = _uri + chunk;
				
				delete[] _uri;
				_uri = NULL;
			}
			if (chunk.find("\r\n") == NPOS) {
				if (ft_strlen(_uri) + chunk.size() > 8192) {
					return (413);
				} else { 
					if (!_uri) {
						_uri = new char[8192];
					}
					strcpy(_uri, chunk.data());
					return (0);
				}
			} else if (ft_strlen(_uri) + chunk.find("\r\n") > 8192) {
				return (413);
			}
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
					if (_headers.find("transfer-encoding") != _headers.end()) {
						_bodysize = "chunked";
						for (std::list<std::string>::const_iterator	it = _headers["transfer-encoding"].begin(); it != _headers["transfer-encoding"].end(); it++) {
							
							if (tolower(*it) == "chunked") {
								_ischunked = true;
								break;
							}
						}
					} if (!_ischunked && _headers.find("content-length") != _headers.end()) {
						if (_headers["content-length"].size() > 1) {
							_bodysize = "0";
						} else {
							_bodysize = _headers["content-length"].front();
							
							for (std::string::const_iterator it = _bodysize.begin(); it != _bodysize.end(); it++) {
								if (!std::isdigit(*it)) {
									_bodysize = "0";
									break ;
								}
							}
						}
					}
					parseBody(chunk);
					if (_bodysize == "Error") {
						return (507);
					} if (_bodysize == "Hexa") {
						return (400);
					} if (_bodysize == "Big") {
						return (413);
					} if (_bodysize != "0" || _case == 3)
						return (201);
					_putfile.close();
					return (200);
				}
				try {
					return(parseHeaders());
				} catch (std::exception & e) {
					
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

std::map<std::string, std::list<std::string> > & Request::getHeaders(void) {
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
