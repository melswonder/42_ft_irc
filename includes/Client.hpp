#pragma once

#include "IRC.hpp"

class Client
{
private:
	int _fd;
	std::string _nickname;
	std::string _username;
	std::string _hostname;
	std::string _realname;
	int _port;
	bool _authenticated;
	bool _registered;
	std::string _buf;
	std::set<std::string> _channels;

public:
	Client();
	Client(int fd);
	~Client();

	// setter
	void setNickname(const std::string &nickname);
	void setUsername(const std::string &username);
	void setHostname(const std::string &hostname);
	void setRealname(const std::string &realname);
	void setPort(const int port);
	void setAuthenticated(bool authenticated);
	void setRegistered(bool registered);

	// getter
	int getFd() const;
	const std::string &getNickname(void) const;
	const std::string &getUsername(void) const;
	const std::string &getHostname(void) const;
	const int &getPort(void) const;
	bool isAuthenticated(void) const;
	bool isRegistered(void) const;
	const std::set<std::string>& getChannels() const;

	void joinChannel(const std::string& channelName);
	void leaveChannel(const std::string& channelName);
	bool isInChannel(const std::string& channelName) const;
	std::string getFullIdentifier() const;

	// メッセージ処理
	// std::vector<std::string> extractCommand(void);
};

std::ostream &operator<<(std::ostream &out, const Client &client);
