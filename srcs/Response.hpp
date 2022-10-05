/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdetune <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/29 12:26:22 by bdetune           #+#    #+#             */
/*   Updated: 2022/10/05 20:00:59 by bdetune          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP
# include <string>
# include <sstream>
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
		int				getTargetFD(void);
		bool			bufferResponse(void);
		bool			headerIsSent(void);
		bool			isOver(void);
		bool			headerBytesSent(std::size_t bytes);
		bool			bodyBytesSent(std::size_t bytes);
		bool			getClose(void);
		bool			getIsConsumed(void);


	private:
		std::string		_header;
		std::size_t		_headerSize;
		std::string		_body;
		std::size_t		_bodySize;
		int				_targetFD;
		bool			_headerSent;
		bool			_chunked;
		bool			_over;
		bool			_FDConsumed;
		bool			_close;
		ServerScope*	_targetServer;

		bool			getServer(std::string const & host, std::vector<ServerScope> & matches);
		void			makeResponse(Request & req);
		bool			findLocation(LocationScope *loc, std::vector<LocationScope> locations, std::string uri);

};

#endif
