#include "../../include/IRC.hpp"

const std::string &Client::getNickname(void) const
{
	return (this->_nickname);
}

const std::string &Client::getUsername(void) const
{
	return (this->_username);
}

bool Client::isAuthenticated(void) const
{
	return (this->_authenticated);
}

bool Client::isRegistered(void) const
{
	return (this->_registered);
}
