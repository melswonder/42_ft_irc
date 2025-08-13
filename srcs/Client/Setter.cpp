#include "../../include/IRC.hpp"

void Client::setNickname(const std::string &nickname)
{
	this->_nickname = nickname;
}

void Client::setUsername(const std::string &username)
{
	this->_username = username;
}

void Client::setAuthenticated(bool authenticated)
{
	this->_authenticated = authenticated;
}

void Client::setRegistered(bool registered)
{
	this->_registered = registered;
}
