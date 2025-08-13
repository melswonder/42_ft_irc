#include "../../../include/IRC.hpp"

// 新しい接続があったとき、STLのfdをpushbackして追加する。
void Server::handleNewConnection()
{
	sockaddr_in clientAddr;
	socklen_t addrLen = sizeof(clientAddr);
	int newFd = 0;
	newFd = accept(_listeningSocketFd, (struct sockaddr *)&clientAddr, &addrLen);
	if (newFd < 0)
	{
		std::cerr << "accept() error: " << std::strerror(errno) << std::endl;
		return;
	}

	if (setNonblocking(newFd) == -1)
		return;
	struct pollfd newPollFd;
	newPollFd.fd = newFd;
	newPollFd.events = POLLIN;
	newPollFd.revents = 0;
	_pollFds.push_back(newPollFd);
	setClientAuthentications(newFd); // 認証
	std::cout << RED << "New connection Fd:" << newPollFd.fd << " !" << WHI << std::endl;
}
