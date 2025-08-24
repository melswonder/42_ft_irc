#include "../../../includes/IRC.hpp"

// === USER ===
void Server::handleUser(Client *client, const std::vector<std::string> &data)
{
	size_t size;

	if (client->isRegistered() == true)
	{
		std::cout << "You already registered!" << std::endl;
		return;
	}
	try
	{
		// if (client->getFd() != fd)
		// 	throw std::runtime_error(INVALID_FD);
		size = data.size();
		if (size < 5)
		{
			if(client->getNickname().empty())
				sendToClient(client->getFd(), getServerPrefix() + ERR_NEEDMOREPARAMS + "* USER :Not enough parameters");
			else
				sendToClient(client->getFd(), getServerPrefix() + ERR_NEEDMOREPARAMS + client->getNickname() + " USER :Not enough parameters");
			throw std::runtime_error(CMD_USER_INVALID_ARGS);
		}
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

		// 認証完了判定（NICK と USER 両方受けているか）
		if (!client->isRegistered()
		&& client->isAuthenticated()
		&& !client->getNickname().empty()
		&& !client->getUsername().empty())
		{
			client->setRegistered(true);
			sendWelcomeMessages(client);
			// getOrCreateChannel(client->getNickname()); もしかしたらつくかも？　irssi側ではnicknameのチャンネルを探していたため
		}
	
	}
	catch (const std::exception &error)
	{
		std::cout << error.what() << std::endl;
		return;
	}

	// std::cout << *this << std::endl;
}
