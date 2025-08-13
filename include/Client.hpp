#pragma once

#include "IRC.hpp"

class Client
{
private:
	int _fd;
	std::string _nickname;
	std::string _username;
	bool _authenticated;
	bool _registered;
	std::string _buf;

public:
	Client();
	Client(int fd);
	~Client();

	// setter
	void setNickname(const std::string &nickname);
	void setUsername(const std::string &username);
	void setNewuser(const int fd, const std::vector<std::string> &data);
	void setAuthenticated(bool authenticated);
	void setRegistered(bool registered);

	// getter
	const std::string &getNickname(void) const;
	const std::string &getUsername(void) const;
	bool isAuthenticated(void) const;
	bool isRegistered(void) const;

	// メッセージ処理
	// std::vector<std::string> extractCommand(void);
};

std::ostream &operator<<(std::ostream &out, const Client &client);
