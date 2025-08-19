#include "../../../includes/IRC.hpp"

// === USER ===
void Server::handleUser(Client* client, const std::vector<std::string> &data)
{
	size_t	size;

	if (client->isRegistered() == true)
	{
		std::cout << "You already registered!" << std::endl;
		return ;
	}
	try
	{
		// if (client->getFd() != fd)
		// 	throw std::runtime_error(INVALID_FD);
		size = data.size();
		if (size < 5)
			throw std::runtime_error(CMD_USER_INVALID_ARGS);
		std::string realname;
		if (data[4][0] != ':')
		{
			realname = data[4];
			for (size_t i = 5; i < size; ++i)
				realname += " " + data[i];
		}
		else
			realname = data[4].substr(1);
		client->setUsername(data[1]);
		client->setRealname(realname);
	}
	catch (const std::exception &error)
	{
		std::cout << error.what() << std::endl;
		return ;
	}

	// ニックネーム変更によって認証が完了するかチェック
	if (!client->isRegistered() && client->isAuthenticated() && !client->getNickname().empty() && !client->getUsername().empty())
	{
		// 認証完了
		client->setRegistered(true);
		// RPL_WELCOME (001) を送信
		sendToClient(client->getFd(), getServerPrefix() + " 001 " + client->getNickname() + " :Welcome to the IRC Network " + client->getFullIdentifier());
	}

	// std::cout << *this << std::endl;
}
