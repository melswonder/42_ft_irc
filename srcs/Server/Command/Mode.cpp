#include "../../../includes/IRC.hpp"

void Server::handleMode(Client* client, const std::vector<std::string> &data)
{
	if (data.size() < 2)
	{
		sendToClient(client->getFd(), getServerPrefix() + ERR_NEEDMOREPARAMS + client->getNickname() + " MODE :Not enough parameters");
		return;
	}

	std::string target = data[1];
	Channel* channel = getChannel(target);
	if (!channel)
	{
		sendToClient(client->getFd(), getServerPrefix() + ERR_NOSUCHCHANNEL + client->getNickname() + " " + target + " :No such channel");
		return;
	}

	// パラメータが2つしかない場合、現在のモードを返す
	if (data.size() == 2)
	{
		// 例: :ircserv 324 <nick> <channel> +itk
		std::string modeString = "+";
		if (channel->isInviteOnly()) modeString += "i";
		if (channel->isTopicRestricted()) modeString += "t";
		if (!channel->getKey().empty()) modeString += "k";
		// RPL_CHANNELMODEIS (324) を送信
		sendToClient(client->getFd(), getServerPrefix() + RPL_CHANNELMODEIS + client->getNickname() + " " + target + " " + modeString);
		return;
	}

	// 変更モードのパース
	std::string modeFlags = data[2];
	bool addingMode = true;
	size_t paramIndex = 3;

	for (size_t i = 0; i < modeFlags.length(); ++i)
	{
		char mode = modeFlags[i];
		if (mode == '+')
		{
			addingMode = true;
			continue;
		}
		else if (mode == '-')
		{
			addingMode = false;
			continue;
		}

		// オペレーター権限の確認が必要なモード
		if (mode == 'o' || mode == 'i' || mode == 't' || mode == 'k' || mode == 'l')
		{
			if (!channel->isOperator(client))
			{
				sendToClient(client->getFd(), getServerPrefix() + ERR_CHANOPRIVSNEEDED + client->getNickname() + " " + target + " :You're not channel operator");
				return;
			}
		}

		// 各モードの処理
		if (mode == 'i')
			channel->setInviteOnly(addingMode);
		else if (mode == 't')
			channel->setTopicRestricted(addingMode);
		else if (mode == 'k')
		{
			if (addingMode)
			{
				if (paramIndex >= data.size())
				{
					sendToClient(client->getFd(), getServerPrefix() + ERR_NEEDMOREPARAMS + client->getNickname() + " MODE :Not enough parameters for +k");
					return;
				}
				channel->setKey(data[paramIndex++]);
			}
			else
			{
				if (paramIndex >= data.size() || channel->getKey() != data[paramIndex])
				{
					// キーが指定されていない、または一致しない場合はエラー
					sendToClient(client->getFd(), getServerPrefix() + ERR_BADCHANNELKEY + client->getNickname() + " " + target + " :Cannot join channel (+k)");
					return;
				}
				channel->setKey(""); // キーを削除
				paramIndex++;
			}
		}
		else if (mode == 'l')
		{
			if (addingMode)
			{
				if (paramIndex >= data.size())
				{
					sendToClient(client->getFd(), getServerPrefix() + ERR_NEEDMOREPARAMS + client->getNickname() + " MODE :Not enough parameters for +l");
					return;
				}
				int limit = atoi(data[paramIndex++].c_str());
				if (limit < 0)
				{
						sendToClient(client->getFd(), getServerPrefix() + ERR_NEEDMOREPARAMS + client->getNickname() + " MODE :Invalid limit value");
						return;
				}
				channel->setUserLimit(limit);
			}
			else
				channel->setUserLimit(0); // 制限を解除
		}
		else if (mode == 'o')
		{
			if (paramIndex >= data.size())
			{
					sendToClient(client->getFd(), getServerPrefix() + ERR_NEEDMOREPARAMS + client->getNickname() + " MODE :Not enough parameters for +o/-o");
					return;
			}
			std::string userNick = data[paramIndex++];
			Client* targetClient = getClientByNickname(userNick);
			if (!targetClient || !channel->isMember(targetClient))
			{
				sendToClient(client->getFd(), getServerPrefix() + ERR_NOSUCHNICK + client->getNickname() + " " + userNick + " :No such nick/channel");
				return;
			}
			if (addingMode)
				channel->addOperator(targetClient);
			else
				channel->removeOperator(targetClient);
		}
	}
}
