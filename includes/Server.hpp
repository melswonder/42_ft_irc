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
	std::map<int, Client*> _clients;
	std::map<std::string, Channel*> _channels;

	void handleNewConnection();
	Situation handleClientData(int clientFd);
	void disconnectClient(int clientFd);
	void serverInfo();
	void serverPing(int clientFd);
	void handlePass(Client* client, const std::vector<std::string> &data);
	void handleNick(Client* client, const std::vector<std::string> &data);
	void handleUser(Client *client, const std::vector<std::string> &data);
	void handleJoin(Client *client, const std::vector<std::string> &data);
	void handlePrivmsg(Client* client, const std::vector<std::string> &data);
	void handleKick(Client* client, const std::vector<std::string> &data);
	void handleMode(Client* client, const std::vector<std::string> &data);
	void handleInvite(Client* client, const std::vector<std::string> &data);
	Channel* getOrCreateChannel(const std::string& channelName);
	Client* getClientByNickname(const std::string& nickname);
	bool isValidNickname(const std::string& nick);

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
	std::map<int, Client*> getClientAuthentications(void) const;

	// Client
	Client* getClient(int fd);
	Channel* getChannel(const std::string& channelName);

	std::string getServerPrefix() const;

	void sendToClient(int clientFd, const std::string& message);
	void broadcastToChannel(const std::string& channelName, const std::string& message, Client* sender = NULL);
	bool isValidChannelName(const std::string& name) const;

	// void setSignal(int signal) {this->_signal = signal;}
};

std::ostream &operator<<(std::ostream &out, const Server &server);
