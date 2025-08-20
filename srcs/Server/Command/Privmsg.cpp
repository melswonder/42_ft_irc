#include "../../../includes/IRC.hpp"

void Server::handlePrivmsg(Client* client, const std::vector<std::string> &data)
{
	if (!client->isRegistered())
	{
		sendToClient(client->getFd(), getServerPrefix() + " 451 " + client->getNickname() + " :You have not registered");
		return;
	}

	if (data.size() < 2)
	{
		sendToClient(client->getFd(), getServerPrefix() + " 411 " + client->getNickname() + " :No recipient given (PRIVMSG)");
		return;
	}

	if (data.size() < 3)
	{
		sendToClient(client->getFd(), getServerPrefix() + " 412 " + client->getNickname() + " :No text to send");
		return;
	}

	std::string targets_str = data[1];
	std::vector<std::string> targets = split(targets_str, ',');
	std::string text;
	if (data[2][0] == ':')
	{
		text = data[2].substr(1);
		for (size_t i = 3; i < data.size(); ++i)
		{
			text += " " + data[i];
		}
	}
	else
	{
		sendToClient(client->getFd(), getServerPrefix() + " 412 " + client->getNickname() + " :No text to send");
		return;
	}

	std::string privmsgMsg = ":" + client->getFullIdentifier() + " PRIVMSG " + targets_str + " :" + text;

	for (size_t i = 0; i < targets.size(); ++i)
	{
		std::string target = targets[i];
		
		if (target[0] == '#' || target[0] == '&') { // チャンネルメッセージ
			Channel* channel = getChannel(target);
			if (!channel)
			{
				sendToClient(client->getFd(), getServerPrefix() + " 401 " + client->getNickname() + " " + target + " :No such nick/channel");
				continue;
			}
			if (!channel->isMember(client))
			{
				sendToClient(client->getFd(), getServerPrefix() + " 404 " + client->getNickname() + " " + target + " :Cannot send to channel");
				continue;
			}
			
			// チャンネル内の全メンバーにメッセージを転送
			broadcastToChannel(target, privmsgMsg, client);
		}
		else // プライベートメッセージ
		{
			Client* targetClient = getClientByNickname(target);
			if (!targetClient)
			{
				sendToClient(client->getFd(), getServerPrefix() + " 401 " + client->getNickname() + " " + target + " :No such nick/channel");
				continue;
			}

			// ターゲットユーザーにメッセージを送信
			sendToClient(targetClient->getFd(), privmsgMsg);
		}
	}
}
