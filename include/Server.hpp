#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h> //-> for socket()
#include <sys/types.h>	//-> for socket()
#include <netinet/in.h> //-> for sockaddr_in
#include <fcntl.h>		//-> for fcntl()
#include <unistd.h>		//-> for close()
#include <arpa/inet.h>	//-> for inet_ntoa()
#include <poll.h>		//-> for poll()
#include <csignal>		//-> for signal()
#include <signal.h>		// signal関数を使うために必要
#include <netdb.h>		// getaddrinfoを使う
#include <cstring>		// memset()
#include <cerrno>		// errno
#include <stdlib.h>		// atoi
#include <algorithm>
#include <map>

#define RED "\033[1;31m" //-> for red color
#define WHI "\033[0;37m" //-> for white color
#define GRE "\033[1;32m" //-> for green color
#define YEL "\033[1;33m" //-> for yellow color

#include "Client.hpp"
#define KEY "hello"

class Server
{

private:
    // bool _signal;
    int _port;
    std::string _password;
    int _listeningSocketFd;
    struct sockaddr_in _server_addr;
    std::vector<struct pollfd> _pollFds;
    // std::map<int, Client> _client;
    std::map<int, Client> _client;

	void handleNewConnection();
	void handleClientData(int clientFd);
	void disconnectClient(int clientFd);
	void checkAuthentication(std::string message, int clientFd);
	void serverInfo();

public:
	Server();
	~Server();
	void serverInit(char *argv[]);
	void serverRun(void);

	void initAddrinfo(in_port_t sin_port, struct in_addr sin_addr);
	int setNonblocking(int fd);
	std::string xorEncryptDecrypt(const std::string& data);

	// setter
	void setPort(int port);
	void setPassword(std::string password);
	void setListeningSocketFd(int listeningSocketFd);
	void setClientAuthentications(int newfd);
	void setServerAddr(int port_nuber);

	// getter
	int getPort(void) const;
	std::string getPassword(void) const;
	int getListeningSocketFd(void) const;
	std::map<int, Client> getClientAuthentications(void) const;

    // Client
    Client getClient(int fd);

    // void setSignal(int signal) {this->_signal = signal;}
};

std::ostream &operator<<(std::ostream &out, const Server &server);
