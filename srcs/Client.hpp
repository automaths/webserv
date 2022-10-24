/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdetune <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/27 16:15:51 by bdetune           #+#    #+#             */
/*   Updated: 2022/10/11 18:27:35 by bdetune          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP
# include "Request.hpp"
# include "Response.hpp"
# include <ctime>
# include <string>
# include <utility>

class Client
{
	public:
		Client(void);
		Client(std::pair<int, unsigned long> const & interface);
		~Client(void);
		Client(Client const & src);

		Client &	operator=(Client const & rhs);

		struct epoll_event &	getEvent(void);
		void					setEvent(struct epoll_event & event);
		int &					getSocketFD(void);
		void					setSocketFD(int fd);
		int &					getPortNumber(void);
		void					setPortNumber(int portNumber);
		std::pair<int, unsigned long>	getInterface(void);
		void					setInterface(std::pair<int, unsigned long>);
		unsigned long &			getIpAddress(void);
		void					setIpAddress(unsigned long address);
		Request &				getRequest(void);
		void					resetRequest(void);
		Response &				getResponse(void);
		void					resetResponse(void);
		int						addToRequest(std::string packet);
		int &					getKeepAlive(void);
		void					setKeepAlive(int keepAlive);
		unsigned int			getNbRequest(void);
		void					addNbRequest(void);
		std::time_t				getLastConnection(void);

	private:
		struct epoll_event	_event;
		int					_socketFD;
		int					_portNumber;
		unsigned long		_ipAddress;
		Request				_request;
		Response			_response;
		int					_keepAlive;
		unsigned int		_nbRequest;
		std::time_t			_lastConnection;
		std::pair<int, unsigned long>	_interface;
};

#endif
