#include "../../../includes/IRC.hpp"

bool Server::isValidChannelName(const std::string& name) const
{
	// 1. 文字列が空か、または規定の最大長を超えていないかチェック
	if (name.empty() || name.length() > 50)
		return false;

	// 2. 最初の文字が '#' または '&' であるかチェック
	char firstChar = name[0];
	if (firstChar != '#' && firstChar != '&')
		return false;

	// 3. チャンネル名に禁止文字が含まれていないかチェック
	for (size_t i = 1; i < name.length(); ++i)
	{
		char c = name[i];
		if (c == ' ' || c == 7 || c == ',')
			return false;
	}

	return true;
}

// === JOIN ===
void Server::handleJoin(Client *client, const std::vector<std::string> &data)
{
	// クライアントが認証済みか確認
	if (!client->isRegistered())
	{
		sendToClient(client->getFd(), getServerPrefix() + ERR_NOTREGISTERED + "* :You have not registered");
		return;
	}

	if (data.size() < 2)
	{
		sendToClient(client->getFd(), getServerPrefix() + ERR_NEEDMOREPARAMS + "* JOIN :Not enough parameters");
		return;
	}

	std::string command = data[0];
	std::vector<std::string> channelNames = split(data[1], ',');
	std::vector<std::string> keys;
	if (data.size() == 3)
		keys = split(data[2], ',');

	for (size_t i = 0; i < channelNames.size(); ++i) {
		std::string channelName = channelNames[i];
		std::string key = "";
		if (keys.size() > i)
			key = keys[i];

		if (!(isValidChannelName(channelName)))
		{
			sendToClient(client->getFd(), getServerPrefix() + ERR_NOSUCHCHANNEL + client->getNickname() + " " + channelName + " :No such channel");
			continue;
		}
	
		// チャンネルの存在を確認
		Channel* channel = getOrCreateChannel(channelName);
	
		if (!channel->canJoin(client, key))
		{
			// 上限
			if (channel->getUserLimit() > 0 && static_cast<int>(channel->getMembers().size()) >= channel->getUserLimit())
				sendToClient(client->getFd(), getServerPrefix() + ERR_CHANNELISFULL + client->getNickname() + " " + channelName + " :Cannot join channel (+l)");
			// キー設定あり
			else if (!channel->getKey().empty() && channel->getKey() != key)
				sendToClient(client->getFd(), getServerPrefix() + ERR_BADCHANNELKEY + client->getNickname() + " " + channelName + " :Cannot join channel (+k)");
			// 招待制
			else if (channel->isInviteOnly() && !channel->isInvited(client))
				sendToClient(client->getFd(), getServerPrefix() + ERR_INVITEONLYCHAN + client->getNickname() + " " + channelName + " :Cannot join channel (+i)");
			continue;
		}
	
		// クライアントをチャンネルに参加させる
		channel->addMember(client);
		// 招待リストから削除
		channel->removeFromInviteList(client);
	
		// 参加したことをクライアントに通知
		std::string joinMsg = ":" + client->getFullIdentifier() + " JOIN :" + channelName;
		broadcastToChannel(channelName, joinMsg);
	
		// トピックがあれば送信
		if (!channel->getTopic().empty())
			sendToClient(client->getFd(), getServerPrefix() + RPL_LIST + client->getNickname() + " " + channelName + " :" + channel->getTopic());
	
		// 名前リストを送信
		sendToClient(client->getFd(), getServerPrefix() + RPL_NAMREPLY + client->getNickname() + " = " + channelName + " :" + channel->getMembersList());
		sendToClient(client->getFd(), getServerPrefix() + RPL_ENDOFNAMES + client->getNickname() + " " + channelName + " :End of /NAMES list");
	}
}
