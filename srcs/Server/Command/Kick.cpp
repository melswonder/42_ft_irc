#include "../../../includes/IRC.hpp"

// 単一のキック処理を実行するヘルパー関数
void Server::executeSingleKickLogic(Server* server, Client* kicker, Channel* channel, Client* target, const std::string& comment)
{
	channel->removeMember(target);

	std::string kickMsg = ":" + kicker->getFullIdentifier() + " KICK " + channel->getName() + " " + target->getNickname();
	if (!comment.empty())
		kickMsg += " :" + comment;

	server->broadcastToChannel(channel->getName(), kickMsg);
	server->sendToClient(target->getFd(), kickMsg);

	if (channel->isEmpty())
	{
		std::map<std::string, Channel*>::iterator it = server->_channels.find(channel->getName());
		if (it != server->_channels.end())
		{
			delete it->second;
			server->_channels.erase(it);
		}
	}
}

// 共通ロジックをまとめたヘルパー関数
void Server::handleKickLogic(Client* client, const std::string& channelName, const std::string& targetNick, const std::string& comment)
{
	// チャンネルの存在チェック
	std::map<std::string, Channel*>::iterator channelIt = _channels.find(channelName);
	if (channelIt == _channels.end())
	{
		sendToClient(client->getFd(), getServerPrefix() + " " + ERR_NOSUCHCHANNEL + " " + client->getNickname() + " " + channelName + " :No such channel");
		return;
	}
	Channel* channel = channelIt->second;

	// 実行者がチャンネルメンバーかチェック
	if (!channel->isMember(client))
	{
		sendToClient(client->getFd(), getServerPrefix() + " " + ERR_NOTONCHANNEL + " " + client->getNickname() + " " + channelName + " :You're not on that channel");
		return;
	}

	// 実行者がチャンネルオペレーターかチェック
	if (!channel->isOperator(client))
	{
		sendToClient(client->getFd(), getServerPrefix() + " " + ERR_CHANOPRIVSNEEDED + " " + client->getNickname() + " " + channelName + " :You're not channel operator");
		return;
	}

	// ターゲットユーザーの存在チェックとチャンネル内メンバーシップの確認
	Client* targetClient = getClientByNickname(targetNick);
	if (!targetClient)
	{ 
		sendToClient(client->getFd(), getServerPrefix() + " " + ERR_NOSUCHNICK + " " + client->getNickname() + " " + targetNick + " :No such nick/channel");
		return;
	}
	if (!channel->isMember(targetClient))
	{
		sendToClient(client->getFd(), getServerPrefix() + " " + ERR_USERNOTINCHANNEL + " " + client->getNickname() + " " + targetNick + " " + channelName + " :They aren't on that channel");
		return;
	}

	// KICKの実行
	executeSingleKickLogic(this, client, channel, targetClient, comment);
}

void Server::handleKick(Client* client, const std::vector<std::string> &data)
{
	if (data.size() < 3)
	{
		sendToClient(client->getFd(), getServerPrefix() + " " + ERR_NEEDMOREPARAMS + " " + client->getNickname() + " KICK :Not enough parameters");
		return;
	}

	// コメント部分のパース
	std::string comment = "";
	if (data.size() > 3)
	{
		if (data[3][0] == ':')
		{
			comment = data[3].substr(1);
			for (size_t i = 4; i < data.size(); ++i)
				comment += " " + data[i];
		}
		else
		{
			if (data.size() == 4)
				comment = data[3];
			else
			{
				sendToClient(client->getFd(), getServerPrefix() + " " + ERR_NEEDMOREPARAMS + " " + client->getNickname() + " KICK :Invalid comment format or too many parameters");
				return;
			}
		}
	}

	std::vector<std::string> channelNames = split(data[1], ',');
	std::vector<std::string> targetNicks = split(data[2], ',');

	if (!((channelNames.size() == 1 && targetNicks.size() >= 1) || (channelNames.size() > 1 && channelNames.size() == targetNicks.size())))
	{
		sendToClient(client->getFd(), getServerPrefix() + " " + ERR_NEEDMOREPARAMS + " " + client->getNickname() + " KICK :Invalid channel/user combination for KICK command");
		return;
	}

	// 複数ユーザーを単一チャンネルからキックする場合
	if (channelNames.size() == 1)
	{
		for (size_t i = 0; i < targetNicks.size(); ++i)
		{
			handleKickLogic(client, channelNames[0], targetNicks[i], comment);
		}
	}
	// 複数チャンネルから対応するユーザーをキックする場合
	else
	{
		for (size_t i = 0; i < channelNames.size(); ++i)
		{
			handleKickLogic(client, channelNames[i], targetNicks[i], comment);
		}
	}
}
