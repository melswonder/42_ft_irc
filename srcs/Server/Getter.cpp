#include "../../include/Server.hpp"

int Server::getPort(void) const
{
	return this->_port;
}

std::string Server::getPassword(void) const
{
	return this->_password;
}

int Server::getListeningSocketFd(void) const
{
	return this->_listeningSocketFd;
}

std::map<int, bool> Server::getClientAuthentications(void) const
{
	return this->_clientAuthentications;
}
