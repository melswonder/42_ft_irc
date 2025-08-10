#include "../../include/Client.hpp"

Client::Client(int fd)
{
	this->_fd = fd;
}

Client::~Client()
{
}

void Client::setNickname(const std::string& nickname)
{
	this->_nickname = nickname;
}

void Client::setUsername(const std::string& username)
{
	this->_username = username;
}

void Client::setHostname(const std::string& hostname)
{
	this->_hostname = hostname;
}

void Client::setRealname(const std::string& realname)
{
	this->_realname = realname;
}

void Client::setAuthenticated(const bool& athenticated)
{
	this->_authenticated = athenticated;
}

void Client::setRegistered(const bool& registered)
{
	this->_registered = registered;
}

int Client::getFd(void) const
{
	return this->_fd;
}

const std::string& Client::getNickname(void) const
{
	return this->_nickname;
}

const std::string& Client::getUserkname(void) const
{
	return this->_username;
}

const std::string& Client::getHostname(void) const
{
	return this->_hostname;
}

const std::string& Client::getRealname(void) const
{
	return this->_realname;
}

bool Client::isAuthenticated(void) const
{
	return this->_authenticated;
}

bool Client::isRegistered(void) const
{
	return this->_registered;
}
