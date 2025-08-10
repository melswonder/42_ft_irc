#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>
#include <set>

class Client
{
private:
	int _fd;
	std::string _nickname;
	std::string _username;
	std::string _hostname;
	std::string _realname;
	bool _authenticated;
	bool _registered;
	std::string _buf;
	std::set<std::string> _channels; // joined channel

public:
	Client(int fd);
	~Client();

	// setter
	void setNickname(const std::string& nickname);
	void setUsername(const std::string& username);
	void setHostname(const std::string& hostname);
	void setRealname(const std::string& realname);
	void setAuthenticated(const bool& athenticated);
	void setRegistered(const bool& registered);

	// getter
	int getFd(void) const;
	const std::string& getNickname(void) const;
	const std::string& getUserkname(void) const;
	const std::string& getHostname(void) const;
	const std::string& getRealname(void) const;

	// 認証状態管理
	bool isAuthenticated(void) const;
	bool isRegistered(void) const;

	// メッセージ処理
	void appendToBuffer(const std::string& data);
	std::vector<std::string> extractCommand(void);
	void sendReply(const std::string& reply);
};

#endif
