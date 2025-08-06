#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client
{
private:
	int _socket;
	std::string _nickname;
	std::string _username;
	std::string _hostname;
	std::string _realname;
	// bool authenticated;
	std::string _buf;


public:
	Client(int socket);
	~Client();

	int getSocket(void) const;
};



#endif
