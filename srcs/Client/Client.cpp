#include "../../include/IRC.hpp"

Client::Client()
{
}

Client::Client(int fd, bool authenticated, bool registered)
{
	this->_fd = fd;
	this->_authenticated = false;
	this->_registered = false;
}

Client::~Client()
{
}

void Client::setNewuser(const int fd, const std::vector<std::string> &data)
{
	size_t	size;

	if (this->_registered == true)
	{
		std::cout << "You already registered!" << std::endl;
		return ;
	}
	try
	{
		if (this->_fd != fd)
			throw std::runtime_error(INVALID_FD);
		size = data.size();
		if (size != 3)
			throw std::runtime_error(CMD_USER_INVALID_ARGS);
		setNickname(data[1]);
		setUsername(data[2]);
		setRegistered(true);
	}
	catch (const std::exception &error)
	{
		std::cout << error.what() << std::endl;
		return ;
	}
	std::cout << *this << std::endl;
}

std::ostream &operator<<(std::ostream &out, const Client &client)
{
	out << "Auth:     " << (client.isAuthenticated() ? "true" : "false") << std::endl;
	out << "Nickname: " << client.getNickname() << std::endl;
	out << "Username: " << client.getUsername() << std::endl;
	return (out);
}
