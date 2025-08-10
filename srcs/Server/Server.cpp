#include "../../include/Server.hpp"

Server::Server() {}
Server::~Server() {}

void Server::serverRun()
{
	std::cout << "Server is running and listening on port " << _port << std::endl;
	while (true)
	{
		int numEvents = poll(&_pollFds[0], _pollFds.size(), -1);

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
			// 既存のクライアントソケットにイベントがあった場合
			else
			{
				if (_pollFds[i].revents & POLLIN)
				{
					handleClientData(_pollFds[i].fd);
				}
				else if (_pollFds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
				{
					std::cerr << "Client disconnected unexpectedly" << std::endl;
					disconnectClient(_pollFds[i].fd);
				}
			}
		}
	}
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
