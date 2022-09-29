/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defaultResponse.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdetune <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/29 13:30:25 by bdetune           #+#    #+#             */
/*   Updated: 2022/09/29 19:24:46 by bdetune          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFAULTRESPONSE_HPP
# define DEFAULTRESPONSE_HPP

# define DEFAULT400BODY "<html>\n<head><title>400 Bad Request</title></head>\n<body bgcolor=\"white\">\n<center><h1>400 Bad Request</h1></center>\n<hr>\n<center><div><img src=\"https://http.cat/400\"  alt=\"A beautiful 400 response\"></div></center>\n<hr>\n<center>webserv/1.0</center>\n</body>\n</html>\n"

# define DEFAULT400STATUS "Bad Request"

# define DEFAULT200STATUS "OK"

# define DEFAULT200BODY "<html>\n<head><title>200 OK</title></head>\n<body bgcolor=\"white\">\n<center><h1>200 OK</h1></center>\n<hr>\n<center><div><img src=\"https://http.cat/200\"  alt=\"A beautiful 200 response\"></div></center>\n<hr>\n<center>webserv/1.0</center>\n</body>\n</html>\n"


#endif
