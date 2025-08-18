#include "../../include/IRC.hpp"

Client::Client() : _fd(-1), _authenticated(false), _registered(false)
{
}

Client::Client(int fd) : _fd(fd), _nickname(""), _username(""), _hostname("localhost"),
	_authenticated(false), _registered(false) {}

Client::~Client() {}

// getter
int Client::getFd() const
{
	return (this->_fd);
}

const std::string &Client::getNickname(void) const
{
	return (this->_nickname);
}

const std::string &Client::getUsername(void) const
{
	return (this->_username);
}

const std::string &Client::getHostname(void) const
{
	return (this->_hostname);
}

bool Client::isAuthenticated(void) const
{
	return (this->_authenticated);
}

bool Client::isRegistered(void) const
{
	return (this->_registered);
}

const std::set<std::string>& Client::getChannels() const {
    return _channels;
}

// setter
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

// メソッド
void Client::joinChannel(const std::string& channelName) {
	this->_channels.insert(channelName);
}

void Client::leaveChannel(const std::string& channelName) {
	this->_channels.erase(channelName);
}

bool Client::isInChannel(const std::string& channelName) const {
	return this->_channels.find(channelName) != _channels.end();
}

void Client::setNewNickname(const int fd, const std::string &nickname)
{
	try
	{
		if (this->_fd != fd)
			throw std::runtime_error(INVALID_FD);
		setNickname(nickname);
		checkAndCompleteRegistration();
	}
	catch (const std::exception &error)
	{
		std::cout << error.what() << std::endl;
		return ;
	}
	std::cout << *this << std::endl;
}

void Client::setNewUsername(const int fd, const std::string &username)
{
	try
	{
		if (this->_fd != fd)
			throw std::runtime_error(INVALID_FD);
		setUsername(username);
		checkAndCompleteRegistration();
	}
	catch (const std::exception &error)
	{
		std::cout << error.what() << std::endl;
		return ;
	}
	std::cout << *this << std::endl;
}

void Client::checkAndCompleteRegistration(void)
{
	if (_authenticated && _authenticated && !_nickname.empty() && !_username.empty())
	{
		setRegistered(true);
		std::cout << "Client " << _fd << " registration completed!" << std::endl;
	}
}

// 出力オペレータのオーバーロード
std::ostream &operator<<(std::ostream &out, const Client &client)
{
	out << "Register: " << (client.isRegistered() ? "true" : "false") << std::endl;
	out << "Auth    : " << (client.isAuthenticated() ? "true" : "false") << std::endl;
	out << "Nickname: " << client.getNickname() << std::endl;
	out << "Username: " << client.getUsername() << std::endl;
	out << "Hostname: " << client.getHostname() << std::endl;
	return (out);
}
