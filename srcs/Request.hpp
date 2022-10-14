/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsartral <nsartral@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/05 17:41:22 by tnaton            #+#    #+#             */
/*   Updated: 2022/10/14 18:22:25 by tnaton           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#define GET "GET"
#define POST "POST"
#define PUT "PUT"
#define DELETE "DELETE"
#define HEAD "HEAD"
#include <string>
#include <map>
#include <vector>
#include <list>
#include <utility>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ftw.h>

class Request{

	public:

		Request(void);
		Request(const Request &);
		~Request(void);
		Request & operator=(const Request &);

		int			moveBody(std::string &);
		void		parseBody(std::string &);
		int			parseHeaders(void);
		int			parseChunk(std::string &);
		int			checkType(std::string &);
		std::string	getType(void) const;
		std::string	getVersion(void) const;
		std::string	getFile(void) const;
		std::string	getBody(void) const;
		std::string	getQuery() const;
		void		setQuery(std::string &);
		void		setFile(std::string &);
		void		setType(std::string &);
		std::map<std::string, std::list<std::string> >	getHeaders(void) const;
		std::vector<std::pair<int, int> >				getRange(int);


		void parseAccept();


	private:

		std::string		_type;
		std::string		_version;
		std::string		_file;
		std::string		_body;
		std::map<std::string, std::list<std::string> > _headers;
		bool			_isbody;
		int				_bodysize;
		std::ofstream	_putfile;
		std::ifstream	_tmpfile;
		std::string		_query;
};
