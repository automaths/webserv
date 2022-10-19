#pragma once

#include <map>
#include <list>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <set>
#include "Client.hpp"
#include "config/scope_server.hpp"
#include <utility>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <csignal>
#define PORT 8080
#define READCHUNKSIZE 1048576
#define WATCHEDEVENTS 100

extern volatile std::sig_atomic_t g_code;

class Server {
    public:

    Server();
    ~Server();

    struct sockaddr_in getAddr(void);
    void initing(std::vector<ServerScope> & virtual_servers);
    void execute(void);

    private:
    
	bool										_filesMoving;
	int											_epoll_fd;			//fd for epoll queue
	char*										_rdBuffer;			//Reading buffer
	std::string									_rdBufferCpy;
	std::map<int, std::vector<ServerScope> >	_virtual_servers;	//List of servers per ports
	std::map<int, int>							_listen_sockets;	//Listening sockets: <fd, port>
	std::map<int, Client>						_client_sockets;	//Accepted connections sockets: <fd, Client>
	std::vector<Client *>						_filesMovingClients;
//	std::set<int>								_reserve_fds;		//Switch between fd to read response from and socket fd to send back data
	std::map<int, int>							_cgi_pipes;			//Cgi pipes
    struct sockaddr_in							_address;			// Address to bind to a socket
	struct epoll_event*							_watchedEvents;
	struct epoll_event							_tmpEv;

	void	moveFiles(void);
	bool	epollSockets(void);
	void	bufferFile(struct epoll_event & event);
	void	acceptIncomingConnection(struct epoll_event & event);
	void	closeTimedoutConnections(void);
	void	readRequest(struct epoll_event & event);
	void	sendResponse(struct epoll_event & event);
	bool	sendHeader(struct epoll_event & event, Client & currentClient);
	void	sendBody(struct epoll_event & event, Client & currentClient);
	void	closeClientSocket(struct epoll_event & event);
	void	readPipe(struct epoll_event & event);
	void	readToWrite(struct epoll_event & event, Client & currentClient, Request & currentRequest, Response & currentResponse);

    class SocketCreationException : public std::exception {virtual const char* what() const throw(){return ("An error occured during socket creation");}};
    class BindException : public std::exception {virtual const char* what() const throw(){return ("An error occured during bind");}};
    class ListenException : public std::exception {virtual const char* what() const throw(){return ("An error occured during listen");}};
    class EpollCreateException : public std::exception {virtual const char* what() const throw(){return ("An error occured while creating an epoll instance");}};

};
