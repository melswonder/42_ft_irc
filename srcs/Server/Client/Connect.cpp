#include "../../../includes/IRC.hpp"

// 新しい接続があったとき、STLのfdをpushbackして追加する。
void Server::handleNewConnection()
{
	sockaddr_in clientAddr;
	socklen_t addrLen = sizeof(clientAddr);
	int clientFd = 0;
	clientFd = accept(_listeningSocketFd, (struct sockaddr *)&clientAddr, &addrLen);
	if (clientFd < 0)
	{
		std::cerr << "accept() error: " << std::strerror(errno) << std::endl;
		return;
	}

	if (setNonblocking(clientFd) == -1)
	{
		close(clientFd);
		return;
	}

	Client *newClient = NULL;
	try
	{
		newClient = new Client(clientFd);
		newClient->setHostname(inet_ntoa(clientAddr.sin_addr));
		newClient->setPort(ntohs(clientAddr.sin_port));

		_clients[clientFd] = newClient;

		struct pollfd newPollFd;
		newPollFd.fd = clientFd;
		newPollFd.events = POLLIN;
		newPollFd.revents = 0;
		_pollFds.push_back(newPollFd);

		setClientAuthentications(clientFd); // 認証
		std::cout << RED << "New connection Fd:" << newPollFd.fd << " !" << WHI << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error creating client: " << e.what() << std::endl;
		if (newClient)
		{
			delete newClient;
		}
		_clients.erase(clientFd);
		close(clientFd);
	}
}
