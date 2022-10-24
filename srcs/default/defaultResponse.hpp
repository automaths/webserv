/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defaultResponse.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdetune <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/29 13:30:25 by bdetune           #+#    #+#             */
/*   Updated: 2022/10/24 12:50:26 by tnaton           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFAULTRESPONSE_HPP
# define DEFAULTRESPONSE_HPP

//200
# define DEFAULT200STATUS "OK"

# define DEFAULT200BODY "<html>\n<head><title>200 OK</title></head>\n<body bgcolor=\"white\">\n<center><h1>200 OK</h1></center>\n<hr>\n<center><div><img src=\"https://http.cat/200\"  alt=\"A beautiful 200 response\"></div></center>\n<hr>\n<center>webserv/1.0</center>\n</body>\n</html>\n"

//201
# define DEFAULT201BODY "<html>\n<head><title>201 Created</title></head>\n<body bgcolor=\"white\">\n<center><h1>201 Created</h1></center>\n<hr>\n<center><div><img src=\"https://http.cat/201\"  alt=\"A beautiful 201 response\"></div></center>\n<hr>\n<center>webserv/1.0</center>\n</body>\n</html>\n"

# define DEFAULT201STATUS "Created"

//307
# define DEFAULT307STATUS "Temporary Redirect"

# define DEFAULT307BODY "<html>\n<head><title>307 Temporary Redirect</title></head>\n<body bgcolor=\"white\">\n<center><h1>307 Temporary Redirect</h1></center>\n<hr>\n<center><div><p>Ressource has moved to : "
# define DEFAULT307BODY2 "</p></div>\n<div><img src=\"https://http.cat/307\"  alt=\"A beautiful 307 response\"></div></center>\n<hr>\n<center>webserv/1.0</center>\n</body>\n</html>\n"

//308
# define DEFAULT308STATUS "Permanent Redirect"

# define DEFAULT308BODY "<html>\n<head><title>308 Permanent Redirect</title></head>\n<body bgcolor=\"white\">\n<center><h1>308 Permanent Redirect</h1></center>\n<hr>\n<center><div><p>Ressource has permanently moved to : "
# define DEFAULT308BODY2 "</p></div>\n<div><img src=\"https://http.cat/308\"  alt=\"A beautiful 308 response\"></div></center>\n<hr>\n<center>webserv/1.0</center>\n</body>\n</html>\n"

//400
# define DEFAULT400BODY "<html>\n<head><title>400 Bad Request</title></head>\n<body bgcolor=\"white\">\n<center><h1>400 Bad Request</h1></center>\n<hr>\n<center><div><img src=\"https://http.cat/400\"  alt=\"A beautiful 400 response\"></div></center>\n<hr>\n<center>webserv/1.0</center>\n</body>\n</html>\n"

# define DEFAULT400STATUS "Bad Request"

//401
# define DEFAULT401BODY "<html>\n<head><title>401 Unauthorized</title></head>\n<body bgcolor=\"white\">\n<center><h1>401 Unauthorized</h1></center>\n<hr>\n<center><div><img src=\"https://http.cat/401\"  alt=\"A beautiful 401 response\"></div></center>\n<hr>\n<center>webserv/1.0</center>\n</body>\n</html>\n"

# define DEFAULT401STATUS "Unauthorized"

//403
# define DEFAULT403BODY "<html>\n<head><title>403 Forbidden</title></head>\n<body bgcolor=\"white\">\n<center><h1>403 Forbidden</h1></center>\n<hr>\n<center><div><img src=\"https://http.cat/403\"  alt=\"A beautiful 403 response\"></div></center>\n<hr>\n<center>webserv/1.0</center>\n</body>\n</html>\n"

# define DEFAULT403STATUS "Forbidden"

//404
# define DEFAULT404BODY "<html>\n<head><title>404 Not Found</title></head>\n<body bgcolor=\"white\">\n<center><h1>404 Not Found</h1></center>\n<hr>\n<center><div><img src=\"https://http.cat/404\"  alt=\"A beautiful 404 response\"></div></center>\n<hr>\n<center>webserv/1.0</center>\n</body>\n</html>\n"

# define DEFAULT404STATUS "Not Found"

//405
# define DEFAULT405BODY "<html>\n<head><title>405 Method Not Allowed</title></head>\n<body bgcolor=\"white\">\n<center><h1>405 Method Not Allowed</h1></center>\n<hr>\n<center><div><img src=\"https://http.cat/405\"  alt=\"A beautiful 405 response\"></div></center>\n<hr>\n<center>webserv/1.0</center>\n</body>\n</html>\n"

# define DEFAULT405STATUS "Method Not Allowed"

//413
# define DEFAULT413BODY "<html>\n<head><title>413 Request Entity Too Large</title></head>\n<body bgcolor=\"white\">\n<center><h1>413 Request Entity Too Large</h1></center>\n<hr>\n<center><div><img src=\"https://http.cat/413\"  alt=\"A beautiful 413 response\"></div></center>\n<hr>\n<center>webserv/1.0</center>\n</body>\n</html>\n"

# define DEFAULT413STATUS "Request Entity Too Large"

//414
# define DEFAULT414BODY "<html>\n<head><title>414 URI Too Long</title></head>\n<body bgcolor=\"white\">\n<center><h1>414 URI Too Long</h1></center>\n<hr>\n<center><div><img src=\"https://http.cat/414\"  alt=\"A beautiful 414 response\"></div></center>\n<hr>\n<center>webserv/1.0</center>\n</body>\n</html>\n"

# define DEFAULT414STATUS "URI Too Long"

//416
# define DEFAULT416BODY "<html>\n<head><title>416 Request Range Not Satisfiable</title></head>\n<body bgcolor=\"white\">\n<center><h1>416 Request Range Not Satisfiable</h1></center>\n<hr>\n<center><div><img src=\"https://http.cat/416\"  alt=\"A beautiful 416 response\"></div></center>\n<hr>\n<center>webserv/1.0</center>\n</body>\n</html>\n"

# define DEFAULT416STATUS "Request Range Not Satisfiable"

//500
# define DEFAULT500BODY "<html>\n<head><title>500 Internal Server Error</title></head>\n<body bgcolor=\"white\">\n<center><h1>500 Internal Server Error</h1></center>\n<hr>\n<center><div><img src=\"https://http.cat/500\"  alt=\"A beautiful 500 response\"></div></center>\n<hr>\n<center>webserv/1.0</center>\n</body>\n</html>\n"

# define DEFAULT500STATUS "Internal Server Error"

//501
# define DEFAULT501BODY "<html>\n<head><title>501 Not Implemented</title></head>\n<body bgcolor=\"white\">\n<center><h1>501 Not Implemented</h1></center>\n<hr>\n<center><div><img src=\"https://http.cat/501\"  alt=\"A beautiful 501 response\"></div></center>\n<hr>\n<center>webserv/1.0</center>\n</body>\n</html>\n"

# define DEFAULT501STATUS "Not Implemented"

//206
# define DEFAULT206STATUS "Partial Content"

# define DEFAULT206BODY "<html>\n<head><title>206 Partial Content</title></head>\n<body bgcolor=\"white\">\n<center><h1>206 Partial Content</h1></center>\n<hr>\n<center><div><img src=\"https://http.cat/206\"  alt=\"A beautiful 206 response\"></div></center>\n<hr>\n<center>webserv/1.0</center>\n</body>\n</html>\n"

//505
# define DEFAULT505BODY "<html>\n<head><title>505 HTTP Version Not Supported</title></head>\n<body bgcolor=\"white\">\n<center><h1>505 HTTP Version Not Supported</h1></center>\n<hr>\n<center><div><img src=\"https://httpcats.com/505.jpg\"  alt=\"A beautiful 505 response\"></div></center>\n<hr>\n<center>webserv/1.0</center>\n</body>\n</html>\n"

# define DEFAULT505STATUS "HTTP Version Not Supported"

//507
# define DEFAULT507BODY "<html>\n<head><title>507 Insufficient Storage</title></head>\n<body bgcolor=\"white\">\n<center><h1>507 Insufficient Storage</h1></center>\n<hr>\n<center><div><img src=\"https://httpcats.com/507.jpg\"  alt=\"A beautiful 507 response\"></div></center>\n<hr>\n<center>webserv/1.0</center>\n</body>\n</html>\n"

# define DEFAULT507STATUS "Insufficient Storage"


#endif
