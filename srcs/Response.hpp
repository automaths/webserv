/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdetune <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/29 12:26:22 by bdetune           #+#    #+#             */
/*   Updated: 2022/10/06 14:13:13 by bdetune          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP
# include <string>
# include <sstream>
# include <fstream>
# include <iostream>
# include <bitset>
# include <ctime>
# include <locale>
# include <vector>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <fcntl.h>
# include "default/defaultResponse.hpp"
# include "config/scope_server.hpp"
# include "Request.hpp"
# include <vector>

class Response
{
	public:
		Response(void);
		Response(Request & req, std::vector<ServerScope> & matches, int error = 0);
		Response(Response const & src);
		~Response(void);

		Response &		operator=(Response const & rhs);

		void			errorResponse(int error);
		void			basicResponse(void);
		std::string		setBaseHeader(void);
		std::string &	getHeader(void);
		std::size_t		getHeaderSize(void);
		std::string &	getBody(void);
		std::size_t		getBodySize(void);
		bool			headerIsSent(void);
		bool			isOver(void);
		bool			headerBytesSent(std::size_t bytes);
		bool			bodyBytesSent(std::size_t bytes);
		bool			getClose(void);
		bool			getIsConsumed(void);
    std::vector<std::string>	getEnv(void) const;
		void			makeResponse(Request & req);

	private:
		std::vector<std::string>	_env;
		std::string		_header;
		std::size_t		_headerSize;
		std::string		_body;
		std::size_t		_bodySize;
		std::ifstream	_targetFile;
		bool			_headerSent;
		bool			_chunked;
		bool			_over;
		bool			_fileConsumed;
		bool			_close;
		ServerScope*	_targetServer;
		char			_responseType;

		bool			getServer(std::string const & host, std::vector<ServerScope> & matches);

		bool			findLocation(LocationScope *loc, std::vector<LocationScope> locations, std::string uri);
};

#endif
