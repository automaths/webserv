/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdetune <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/27 16:15:51 by bdetune           #+#    #+#             */
/*   Updated: 2022/10/04 12:38:35 by bdetune          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP
# include "Request.hpp"
# include "Response.hpp"
# include <ctime>
# include <string>

class Client
{
	public:
		Client(void);
		Client(int const & portNumber);
		~Client(void);
		Client(Client const & src);

		Client &	operator=(Client const & rhs);

		int &			getPortNumber(void);
		void			setPortNumber(int portNumber);
		Request &		getRequest(void);
		void			resetRequest(void);
		Response &		getResponse(void);
		void			resetResponse(void);
		int				addToRequest(std::string packet);
		int &			getKeepAlive(void);
		void			setKeepAlive(int keepAlive);
		unsigned int	getNbRequest(void);
		void			addNbRequest(void);
		std::time_t		getLastConnection(void);

	private:
		int				_portNumber;
		Request			_request;
		Response		_response;
		int				_keepAlive;
		unsigned int	_nbRequest;
		std::time_t		_lastConnection;
};

#endif
