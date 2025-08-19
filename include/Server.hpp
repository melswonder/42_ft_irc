#pragma once

#include "IRC.hpp"
#define KEY "hello"

enum Situation {
	CONNECT,
	DISCONNECT
};

class Server
{
private:
	// bool _signal;
	int _port;
	std::string _password;
	int _listeningSocketFd;
	struct sockaddr_in _server_addr;
	std::vector<struct pollfd> _pollFds;
	std::map<int, Client> _client;
	std::map<int, Channel> _channel;

	void handleNewConnection();
	Situation handleClientData(int clientFd);
	void disconnectClient(int clientFd);
	void checkAuthentication(std::string pasword, int clientFd);
	void serverInfo();
	void serverPing(int clientFd);
	void setNewuser(Client *client, const std::vector<std::string> &data);

public:

	Server();
	~Server();
	void serverInit(char *argv[]);
	void serverRun(void);

	void initAddrinfo(in_port_t sin_port, struct in_addr sin_addr);
	int setNonblocking(int fd);
	std::string xorEncryptDecrypt(const std::string &data);

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
	Client* getClient(int fd);

	// void setSignal(int signal) {this->_signal = signal;}
};

std::ostream &operator<<(std::ostream &out, const Server &server);
