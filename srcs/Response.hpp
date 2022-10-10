/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsartral <nsartral@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/29 12:26:22 by bdetune           #+#    #+#             */
/*   Updated: 2022/10/10 18:24:58 by nsartral         ###   ########.fr       */
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
# include "MimeTypes.hpp"
# include <vector>
# include "ListDirectory.hpp"

class Response
{
	public:
		Response(void);
		Response(Request & req, std::vector<ServerScope> & matches, int error = 0);
		Response(Response const & src);
		~Response(void);

		Response &		operator=(Response const & rhs);

		void			errorResponse(int error);
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

		int				execCgi(std::string exec);
		void			cgiResponse(int fd);

	private:
		std::vector<std::string>	_env;
		std::string		_header;
		std::size_t		_headerSize;
		std::string		_body;
		std::size_t		_bodySize;
		std::ifstream	_targetFile;
		std::string		_targetFilePath;
		bool			_headerSent;
		bool			_chunked;
		bool			_over;
		bool			_fileConsumed;
		bool			_close;
		ServerScope*	_targetServer;
		char			_responseType;

		//tmp of the fullPath string to use it in cgi function (in create response)
		bool			_is_cgi;
		int				_cgi_fd;
		int				_cgi_input;

		bool			getServer(std::string const & host, std::vector<ServerScope> & matches);

		bool			findLocation(LocationScope *loc, std::vector<LocationScope> locations, std::string uri);
		bool			allowedMethod(std::vector<std::string> methods, std::string currentMethod);
		bool			pathIsValid(std::string path, struct stat * buf);
		bool			foundDirectoryIndex(std::vector<std::string> indexes, std::string path);
		std::string		createFileResponse(void);
		void			create200Header(std::string extension);
		void			createFileErrorHeader(int errorCode, std::string mime);
		bool			openFile(std::string path);
};

#endif
