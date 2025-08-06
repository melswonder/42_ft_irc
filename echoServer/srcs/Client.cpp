#include "../include/Client.hpp"

Client::Client(int socket)
{
	this->_socket = socket;
}

Client::~Client()
{
}

int Client::getSocket(void) const
{
	return this->_socket;
}
