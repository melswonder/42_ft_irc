#include "../../../includes/IRC.hpp"

bool Server::isValidChannelName(const std::string& name) const
{
	// 1. 文字列が空か、または規定の最大長を超えていないかチェック
	if (name.empty() || name.length() > 200)
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
		sendToClient(client->getFd(), getServerPrefix() + " 451 * :You have not registered");
		return;
	}

	if (data.size() < 2)
	{
		sendToClient(client->getFd(), getServerPrefix() + " 461 * JOIN :Not enough parameters");
		return;
	}

	std::string command = data[0];
	std::string channelName = data[1];
	std::string key;
	if (data.size() == 3)
		key = data[2];

	if (!(isValidChannelName(channelName)))
	{
		sendToClient(client->getFd(), getServerPrefix() + " 403 " + client->getNickname() + " " + channelName + " :No such channel");
		return;
	}

	// チャンネルの存在を確認
	Channel* channel = getOrCreateChannel(channelName);

	if (!channel->canJoin(client, key))
	{
		// 上限
		if (channel->getUserLimit() > 0 && static_cast<int>(channel->getMembers().size()) >= channel->getUserLimit())
			sendToClient(client->getFd(), getServerPrefix() + " 471 " + client->getNickname() + " " + channelName + " :Cannot join channel (+l)");
		// キー設定あり
		else if (!channel->getKey().empty() && channel->getKey() != key)
			sendToClient(client->getFd(), getServerPrefix() + " 475 " + client->getNickname() + " " + channelName + " :Cannot join channel (+k)");
		// 招待制
		else if (channel->isInviteOnly() && !channel->isInvited(client))
			sendToClient(client->getFd(), getServerPrefix() + " 473 " + client->getNickname() + " " + channelName + " :Cannot join channel (+i)");
		return;
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
		sendToClient(client->getFd(), getServerPrefix() + " 332 " + client->getNickname() + " " + channelName + " :" + channel->getTopic());

	// 名前リストを送信
	sendToClient(client->getFd(), getServerPrefix() + " 353 " + client->getNickname() + " = " + channelName + " :" + channel->getMembersList());
	sendToClient(client->getFd(), getServerPrefix() + " 366 " + client->getNickname() + " " + channelName + " :End of /NAMES list");
}
