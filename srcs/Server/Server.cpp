#include "../../include/IRC.hpp"

Server::Server() {}
Server::~Server() {}

void Server::serverRun()
{
	std::cout << "Server is running and listening on port " << _port << std::endl;
	int numEvents = 0;
	while (true)
	{
		numEvents = poll(&_pollFds[0], _pollFds.size(), -1);

		if (numEvents < 0)
		{
			if (errno == EINTR)
				continue;
			std::cerr << "poll() error" << std::endl;
			break;
		}

		// numEventsが0の場合(タイムアウト時)の処理を省略
		for (size_t i = 0; i < _pollFds.size(); ++i)
		{
			if (_pollFds[i].revents == 0)
				continue;

			// 接続街のソケットからのイベント
			if (_pollFds[i].fd == _listeningSocketFd)
			{
				if (_pollFds[i].revents & POLLIN)
					handleNewConnection();
			}
			else
			{
				if (_pollFds[i].revents & POLLIN)
				{
					if (handleClientData(_pollFds[i].fd) == DISCONNECT)
						i--; //本来ユーザー側からEXITするコマンドの実装の意味があるのかは不明
				}
				else if (_pollFds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
				{
					std::cerr << "Client disconnected unexpectedly" << std::endl;
					disconnectClient(_pollFds[i].fd);
					i--;
				}
			}
		}
	}
}

//===setter===
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

//===getter===
int Server::getPort(void) const
{
	return (this->_port);
}

std::string Server::getPassword(void) const
{
	return (this->_password);
}

int Server::getListeningSocketFd(void) const
{
	return (this->_listeningSocketFd);
}

std::map<int, Client> Server::getClientAuthentications(void) const
{
	return (this->_client);
}

Client* Server::getClient(int fd)
{
	std::cout << "getClient()" << std::endl;
	std::map<int, Client>::iterator it = _client.find(fd);
	if (it != _client.end())
		return &(it->second);
	return NULL;
}


std::ostream &operator<<(std::ostream &out, const Server &server)
{
	out << "Port to bind: " << server.getPort() << std::endl
		<< "Password :" << server.getPassword() << std::endl
		<< "Serverfd :" << server.getListeningSocketFd() << std::endl;
	const std::map<int, Client> &authentications = server.getClientAuthentications();
	for (std::map<int, Client>::const_iterator it = authentications.begin();
		it != authentications.end(); it++)
		out << "fd[" << it->first << "] 認証 :" << (it->second.isAuthenticated() ? "有効" : "無効") << std::endl;
	return out;
}
