#include "../../../includes/IRC.hpp"

// === USER ===
void Server::handleUser(Client *client, const std::vector<std::string> &data)
{
	size_t size = 0;

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
		std::string serverName = data[3];

	// 001
	// clientオブジェクトからユーザー名とホスト名を動的に取得
	std::string user_host = client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname();
	std::string welcomeMsg = "Welcome to the ft_irc network, " + user_host;
	sendNumericReply(client->getFd(), serverName, client->getNickname(), "001", welcomeMsg);

	// 002
	sendNumericReply(client->getFd(), serverName, client->getNickname(), "002", "Your host is " + serverName + ", running version ft_irc-1.0");

	// 003
	time_t now = time(0);
	std::string createdTime = ctime(&now);
	// ctime()が追加する末尾の改行を削除
	createdTime.erase(createdTime.find_last_not_of(" \n\r\t")+1);
	sendNumericReply(client->getFd(), serverName, client->getNickname(), "003", "This server was created " + createdTime);

	// 004
	sendNumericReply(client->getFd(), serverName, client->getNickname(), "004", serverName + " ft_irc-1.0 i t k o l");

	// 005 (改善案)
	sendNumericReply(client->getFd(), serverName, client->getNickname(), "005", "CHANMODES=i,t,k,o,l PREFIX=(o)@ CHANTYPES=# :are supported by this server");

	// MOTD (Message of the Day)
	sendNumericReply(client->getFd(), serverName, client->getNickname(), "375", "- " + serverName + " Message of the day -");
	sendNumericReply(client->getFd(), serverName, client->getNickname(), "372", "- Welcome to the ft_irc server!");
	sendNumericReply(client->getFd(), serverName, client->getNickname(), "372", "- Please follow the rules.");
	sendNumericReply(client->getFd(), serverName, client->getNickname(), "376", "End of MOTD command");
	}
	catch (const std::exception &error)
	{
		std::cout << error.what() << std::endl;
		return;
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
