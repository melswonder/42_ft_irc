#include "../../include/IRC.hpp"

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
