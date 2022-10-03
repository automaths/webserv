#pragma once

#include <map>
#include <list>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "Client.hpp"
#include <utility>
#include <ctime>
#include <cstring>
#include <csignal>
#define PORT 8080

extern volatile std::sig_atomic_t g_code;

class Server {
    public:

    Server();
    ~Server();

    struct sockaddr_in getAddr(void);
    void initing(void);
    void execute(void);


    private:
    
	int						_epoll_fd;			//fd for epoll queue
	char					_rdBuffer[1048577];	//Reading buffer
	std::map<int, int>		_listen_sockets;	//Listening sockets: <fd, port>
	std::map<int, Client>	_client_sockets;	//Accepted connections sockets: <fd, Client>
    struct sockaddr_in		_address;			// Address to bind to a socket

	bool	epollSockets(void);
	void	acceptIncomingConnection(struct epoll_event & event);
	void	closeTimedoutConnections(void);
	void	readRequest(struct epoll_event & event);

    class SocketCreationException : public std::exception {virtual const char* what() const throw(){return ("An error occured during socket creation");}};
    class BindException : public std::exception {virtual const char* what() const throw(){return ("An error occured during bind");}};
    class ListenException : public std::exception {virtual const char* what() const throw(){return ("An error occured during listen");}};
    class EpollCreateException : public std::exception {virtual const char* what() const throw(){return ("An error occured while creating an epoll instance");}};

};
