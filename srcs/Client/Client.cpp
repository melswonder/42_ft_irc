#include "../../include/IRC.hpp"

Client::Client() : _fd(-1), _authenticated(false), _registered(false)
{
}

Client::Client(int fd) : _fd(fd), _authenticated(false), _registered(false)
{
}

Client::~Client()
{
}

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

// 出力オペレータのオーバーロード
std::ostream &operator<<(std::ostream &out, const Client &client)
{
	out << "Auth:     " << (client.isAuthenticated() ? "true" : "false") << std::endl;
	out << "Nickname: " << client.getNickname() << std::endl;
	out << "Username: " << client.getUsername() << std::endl;
	return (out);
}
