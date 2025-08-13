#include "../../include/IRC.hpp"

void Server::setPort(int port)
{
	this->_port = port;
}

void Server::setPassword(std::string password)
{
	this->_password = password;
}

void Server::setListeningSocketFd(int listeningSocketFd)
{
	this->_listeningSocketFd = listeningSocketFd;
}

void Server::setClientAuthentications(int newfd)
{
	Client newClient(newfd);
	_client.insert(std::make_pair(newfd, newClient));
	std::map<int, Client>::iterator it = _client.find(newfd);
	it->second.setAuthenticated(false);
	std::cout << "Created new client for fd: " << newfd << std::endl;
}

// この関数はヘルパー関数です　後にbind()という関数で、
// ソケット通信に必要な情報を整理し、関数に渡すための手段を踏んでいます
void Server::setServerAddr(int port_number)
{
	memset(&_server_addr, 0, sizeof(_server_addr));
	_server_addr.sin_family = AF_INET;          // IPv4の使用
	_server_addr.sin_addr.s_addr = INADDR_ANY;  // どのIPからも接続を許可
	_server_addr.sin_port = htons(port_number); // ポート番号設定
}
