#include "../../include/IRC.hpp"

void Client::setNickname(const std::string& nickname)
{
	this->_nickname = nickname;
}

void Client::setUsername(const std::string& username)
{
	this->_username = username;
}

void Client::setAuthenticated(const bool& athenticated)
{
	this->_authenticated = athenticated;
}

void Client::setRegistered(const bool& registered)
{
	this->_registered = registered;
}
