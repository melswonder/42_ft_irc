#include "../../../includes/IRC.hpp"

void Server::handleKick(Client* client, const std::vector<std::string> &data)
{
	// パラメータのチェック: ERR_NEEDMOREPARAMS (461)
	if (data.size() < 3)
	{
		sendToClient(client->getFd(), getServerPrefix() + " 461 " + client->getNickname() + " KICK :Not enough parameters");
		return;
	}

	std::string channelName = data[1];
	std::string targetNick = data[2];
	std::string comment = (data.size() > 3) ? data[3] : "";

	// コメントにコロンが含まれているかチェックし、再構築
	if (data.size() > 3 && data[3][0] == ':')
	{
		comment = data[3].substr(1);
		for (size_t i = 4; i < data.size(); ++i)
			comment += " " + data[i];
	}
	else
	{
		// コロンがない場合、プロトコル違反とみなし、エラーを返す
		// この場合、ERR_NEEDMOREPARAMS (461) を返す
		sendToClient(client->getFd(), getServerPrefix() + " 461 " + client->getNickname() + " KICK :Not enough parameters");
		return;
	}

	// チャンネルの存在チェック: ERR_NOSUCHCHANNEL (403)
	std::map<std::string, Channel*>::iterator channelIt = _channels.find(channelName);
	if (channelIt == _channels.end())
	{
		sendToClient(client->getFd(), getServerPrefix() + " 403 " + client->getNickname() + " " + channelName + " :No such channel");
		return;
	}

	Channel* channel = channelIt->second;

	// 実行者がチャンネルメンバーかチェック: ERR_NOTONCHANNEL (442)
	if (!channel->isMember(client))
	{
		sendToClient(client->getFd(), getServerPrefix() + " 442 " + client->getNickname() + " " + channelName + " :You're not on that channel");
		return;
	}

	// 実行者がチャンネルオペレーターかチェック: ERR_CHANOPRIVSNEEDED (482)
	if (!channel->isOperator(client))
	{
		sendToClient(client->getFd(), getServerPrefix() + " 482 " + client->getNickname() + " " + channelName + " :You're not channel operator");
		return;
	}

	// ターゲットユーザーの存在チェック: ERR_NOSUCHNICK (401)
	// ターゲットがチャンネル内にいるかどうかも確認
	Client* targetClient = getClientByNickname(targetNick);
	if (!targetClient || !channel->isMember(targetClient))
	{
		sendToClient(client->getFd(), getServerPrefix() + " 401 " + client->getNickname() + " " + targetNick + " :No such nick/channel");
		return;
	}

	// KICKの実行
	// チャンネルからターゲットを削除
	channel->removeMember(targetClient);

	// チャンネルメンバーとターゲットに通知
	std::string kickMsg = ":" + client->getFullIdentifier() + " KICK " + channelName + " " + targetNick + " :" + comment;
	if (comment.empty())
		kickMsg = ":" + client->getFullIdentifier() + " KICK " + channelName + " " + targetNick;

	// チャンネル全員にキックメッセージをブロードキャスト
	broadcastToChannel(channelName, kickMsg);

	// キックされたユーザーにも通知
	sendToClient(targetClient->getFd(), kickMsg);

	// チャンネルが空になったら削除する
	if (channel->isEmpty())
	{
		delete channel; // チャンネルを削除
		_channels.erase(channelIt); // マップから削除
	}
}
