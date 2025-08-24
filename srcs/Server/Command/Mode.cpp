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
		std::string modeString = "+";
		if (channel->isInviteOnly()) modeString += "i";
		if (channel->isTopicRestricted()) modeString += "t";
		if (!channel->getKey().empty()) modeString += "k";
		if (channel->getUserLimit() > 0) modeString += "l";
		// RPL_CHANNELMODEIS (324) を送信
		sendToClient(client->getFd(), getServerPrefix() + RPL_CHANNELMODEIS + client->getNickname() + " " + target + " " + modeString);
		return;
	}

	// 変更モードのパース - data[2]以降をすべて処理する
	bool addingMode = true;
	size_t dataIndex = 2;
	
	// 成功したモード変更を記録
	std::string appliedModes = "";
	std::vector<std::string> appliedParams;
	bool currentSign = true;
	bool needSignInOutput = true;

	while (dataIndex < data.size())
	{
		std::string token = data[dataIndex];
		
		if (token.empty() || (token[0] != '+' && token[0] != '-'))
		{
			dataIndex++;
			continue;
		}
	
		dataIndex++;
		
		for (size_t i = 0; i < token.length(); ++i)
		{
			char mode = token[i];
		
			if (mode == '+')
			{
				addingMode = true;
				if (currentSign != true)
				{
					currentSign = true;
					needSignInOutput = true;
				}
				continue;
			}
			else if (mode == '-')
			{
				addingMode = false;
				if (currentSign != false)
				{
					currentSign = false;
					needSignInOutput = true;
				}
				continue;
			}

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
			{
				channel->setInviteOnly(addingMode);
				if (needSignInOutput)
				{
					appliedModes += (addingMode ? "+" : "-");
					needSignInOutput = false;
				}
				appliedModes += mode;
			}
			else if (mode == 't')
			{
				channel->setTopicRestricted(addingMode);
				if (needSignInOutput)
				{
					appliedModes += (addingMode ? "+" : "-");
					needSignInOutput = false;
				}
				appliedModes += mode;
			}
			else if (mode == 'k')
			{
				if (addingMode)
				{
					if (dataIndex >= data.size())
					{
						sendToClient(client->getFd(), getServerPrefix() + " 461 " + client->getNickname() + " MODE :Not enough parameters for +k");
						continue;
					}
					std::string key = data[dataIndex++];
					channel->setKey(key);
					if (needSignInOutput)
					{
						appliedModes += "+";
						needSignInOutput = false;
					}
					appliedModes += mode;
					appliedParams.push_back(key);
				}
				else
				{
					if (dataIndex >= data.size())
					{
						sendToClient(client->getFd(), getServerPrefix() + " 461 " + client->getNickname() + " MODE :Not enough parameters for -k");
						continue;
					}
					std::string providedKey = data[dataIndex++];
					if (channel->getKey() != providedKey)
					{
						sendToClient(client->getFd(), getServerPrefix() + " 475 " + client->getNickname() + " " + target + " :Cannot remove channel key (wrong key)");
						continue;
					}
					channel->setKey("");
					if (needSignInOutput)
					{
						appliedModes += "-";
						needSignInOutput = false;
					}
					appliedModes += mode;
					appliedParams.push_back(providedKey);
				}
			}
			else if (mode == 'l')
			{
				if (addingMode)
				{
					if (dataIndex >= data.size())
					{
						sendToClient(client->getFd(), getServerPrefix() + " 461 " + client->getNickname() + " MODE :Not enough parameters for +l");
						continue;
					}
					std::string limitStr = data[dataIndex++];
					int limit = atoi(limitStr.c_str());
					if (limit <= 0)
					{
						sendToClient(client->getFd(), getServerPrefix() + " 461 " + client->getNickname() + " MODE :Invalid limit value");
						continue;
					}
					channel->setUserLimit(limit);
					if (needSignInOutput)
					{
						appliedModes += "+";
						needSignInOutput = false;
					}
					appliedModes += mode;
					appliedParams.push_back(limitStr);
				}
				else
				{
					channel->setUserLimit(0);
					if (needSignInOutput)
					{
						appliedModes += "-";
						needSignInOutput = false;
					}
					appliedModes += mode;
				}
			}
			else if (mode == 'o')
			{
				if (dataIndex >= data.size())
				{
					sendToClient(client->getFd(), getServerPrefix() + " 461 " + client->getNickname() + " MODE :Not enough parameters for +o/-o");
					continue;
				}
				std::string userNick = data[dataIndex++];
				Client* targetClient = getClientByNickname(userNick);
				if (!targetClient)
				{
					sendToClient(client->getFd(), getServerPrefix() + " 401 " + client->getNickname() + " " + userNick + " :No such nick/channel");
					continue;
				}
				if (!channel->isMember(targetClient))
				{
					sendToClient(client->getFd(), getServerPrefix() + " 441 " + client->getNickname() + " " + userNick + " " + target + " :They aren't on that channel");
					continue;
				}
				
				if (addingMode)
				{
					if (!channel->isOperator(targetClient))
					{
						channel->addOperator(targetClient);
						if (needSignInOutput)
						{
							appliedModes += "+";
							needSignInOutput = false;
						}
						appliedModes += mode;
						appliedParams.push_back(userNick);
					}
				}
				else
				{
					if (channel->isOperator(targetClient))
					{
						channel->removeOperator(targetClient);
						if (needSignInOutput)
						{
							appliedModes += "-";
							needSignInOutput = false;
						}
						appliedModes += mode;
						appliedParams.push_back(userNick);
					}
				}
			}
			else
				sendToClient(client->getFd(), getServerPrefix() + " 472 " + client->getNickname() + " " + mode + " :is unknown mode char to me");
		}
	}

	// 実際に適用されたモード変更があれば、チャンネルのメンバーに通知
	if (!appliedModes.empty())
	{
		std::string modeResponse = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " MODE " + target + " " + appliedModes;
		
		// パラメータがあれば追加
		for (size_t i = 0; i < appliedParams.size(); ++i) {
			modeResponse += " " + appliedParams[i];
		}
		
		// チャンネルの全メンバーに送信
		broadcastToChannel(target, modeResponse, NULL);
	}
}