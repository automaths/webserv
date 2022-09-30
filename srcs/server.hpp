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

//	bool	acceptIncomingConnection(struct epoll_event event[i])

    void execute(void);

	void	closeTimedoutConnections(void);

    private:
    
	int						_epoll_fd;			//fd for epoll queue
	std::map<int, int>		_listen_sockets;	//Listening sockets: <fd, port>
	std::map<int, Client>	_client_sockets;	//Accepted connections sockets: <fd, Client>
    struct sockaddr_in		_address;			// Address to bind to a socket

	bool	epollSockets(void);
    class SocketCreationException : public std::exception {virtual const char* what() const throw(){return ("An error occured during socket creation");}};
    class BindException : public std::exception {virtual const char* what() const throw(){return ("An error occured during bind");}};
    class ListenException : public std::exception {virtual const char* what() const throw(){return ("An error occured during listen");}};
    class SelectException : public std::exception {virtual const char* what() const throw(){return ("An error occured during select");}};
    class EpollCreateException : public std::exception {virtual const char* what() const throw(){return ("An error occured while creating an epoll instance");}};
    class AcceptException : public std::exception {virtual const char* what() const throw(){return ("An error occured during accept");}};
    class ReadException : public std::exception {virtual const char* what() const throw(){return ("An error occured during read");}};
    class SendException : public std::exception {virtual const char* what() const throw(){return ("An error occured during send");}};

};
