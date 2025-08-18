#include "../../../include/IRC.hpp"

//=== PASS ===
void Server::checkAuthentication(std::string password, int clientFd)
{
	password.erase(password.find_last_not_of(" \r\n\t") + 1);
	password.erase(0, password.find_first_not_of(" \r\n\t"));
	password = xorEncryptDecrypt(password);
	// std::cout << "Serverpass[" << _password << "]len:" << _password.length() << std::endl
	// 			<< "Clientpass[" << password << "]len:" << _password.length() << std::endl;
	if (_password == password)
	{
		std::cout << "認証OK!" << std::endl;
		std::map<int, Client>::iterator it = _client.find(clientFd);
		if (it != _client.end())
		{
			std::cout << "Password verified!" << std::endl;
			it->second.setAuthenticated(true);
		}
	}
	else
	{
		std::cout << "認証KO" << std::endl;
	}
}

// === INFO ===
void Server::serverInfo()
{
	std::cout << *this << std::endl;
}

// === EXIT === 
void Server::disconnectClient(int clientFd)
{
	for (size_t i = 0; i < _pollFds.size(); ++i)
	{
		if (_pollFds[i].fd == clientFd)
		{
			_pollFds.erase(_pollFds.begin() + i);
			break;
		}
	}
	close(clientFd);
}

// === PING ===
void Server::serverPing(int clientFd)
{
	std::string response = "PONG\n";
	send(clientFd, response.c_str(), response.length(), 0);
}

// === USER ===
void Server::setNewuser(Client* client, const std::vector<std::string> &data)
{
	size_t	size;

	if (client->isAuthenticated() == true)
	{
		std::cout << "You already registered!" << std::endl;
		return ;
	}
	try
	{
		// if (client->getFd() != fd)
		// 	throw std::runtime_error(INVALID_FD);
		size = data.size();
		if (size != 3)
			throw std::runtime_error(CMD_USER_INVALID_ARGS);
		client->setNickname(data[1]);
		client->setUsername(data[2]);
		client->setRegistered(true);
	}
	catch (const std::exception &error)
	{
		std::cout << error.what() << std::endl;
		return ;
	}
	std::cout << *this << std::endl;
}

//パスワード認証が終わったらこのレスポンスを返さなければいけない
// Received: CAP LS
// NICK hirwatan
// USER hirwatan hirwatan localhost :Hiro Watanabe
