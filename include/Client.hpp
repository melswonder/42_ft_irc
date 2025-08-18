#pragma once

#include "IRC.hpp"

class Client
{
private:
	int _fd;
	std::string _nickname;
	std::string _username;
	std::string _hostname;
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
	void setNewNickname(const int fd, const std::string &username);
	void setNewUsername(const int fd, const std::string &username);
	void setAuthenticated(bool authenticated);
	void setRegistered(bool registered);

	// getter
	int getFd() const;
	const std::string &getNickname(void) const;
	const std::string &getUsername(void) const;
	const std::string &getHostname(void) const;

	bool isAuthenticated(void) const;
	bool isRegistered(void) const;
	const std::set<std::string>& getChannels() const;

	void joinChannel(const std::string& channelName);
	void leaveChannel(const std::string& channelName);
	bool isInChannel(const std::string& channelName) const;

	void checkAndCompleteRegistration(void);

	// メッセージ処理
	// std::vector<std::string> extractCommand(void);
};

std::ostream &operator<<(std::ostream &out, const Client &client);
