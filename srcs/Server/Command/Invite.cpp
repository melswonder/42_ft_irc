#include "../../../includes/IRC.hpp"

void Server::handleInvite(Client* client, const std::vector<std::string> &data) {
	// パラメータのチェック: ERR_NEEDMOREPARAMS
	if (data.size() < 3)
	{
		sendToClient(client->getFd(), getServerPrefix() + ERR_NEEDMOREPARAMS + client->getNickname() + " INVITE :Not enough parameters");
		return;
	}

	std::string targetNickname = data[1];
	std::string channelName = data[2];

	// ターゲットユーザーの存在確認: ERR_NOSUCHNICK
	Client* targetClient = getClientByNickname(targetNickname);
	if (!targetClient)
	{
		sendToClient(client->getFd(), getServerPrefix() + ERR_NOSUCHNICK + client->getNickname() + " " + targetNickname + " :No such nick/channel");
		return;
	}

	// チャンネルの存在と、招待者がチャンネルにいるか確認: ERR_NOTONCHANNEL
	Channel* channel = getChannel(channelName);
	if (channel && !channel->isMember(client))
	{
		sendToClient(client->getFd(), getServerPrefix() + ERR_NOMOTD + client->getNickname() + " " + channelName + " :You're not on that channel");
		return;
	}

	// チャンネルが招待制の場合、招待者がオペレーターか確認: ERR_CHANOPRIVSNEEDED
	if (channel && channel->isInviteOnly() && !channel->isOperator(client))
	{
		sendToClient(client->getFd(), getServerPrefix() + ERR_CHANOPRIVSNEEDED + client->getNickname() + " " + channelName + " :You're not channel operator");
		return;
	}

	// ターゲットユーザーが既にチャンネルにいるか確認: ERR_USERONCHANNEL
	if (channel && channel->isMember(targetClient)) 
	{
		sendToClient(client->getFd(), getServerPrefix() + ERR_NICKNAMEINUSE + client->getNickname() + " " + targetNickname + " " + channelName + " :is already on channel");
		return;
	}

	// 招待の実行
	// チャンネルが存在しない場合は、作成される
	if (!channel)
		channel = getOrCreateChannel(channelName);

	// 招待リストに追加
	channel->addToInviteList(targetClient);

	// 送信者と招待されたユーザーにメッセージを送信
	// RPL_INVITING (341) を送信者に送る
	sendToClient(client->getFd(), getServerPrefix() + RPL_INVITING + client->getNickname() + " " + targetNickname + " " + channelName);

	// 招待されたユーザーにINVITEメッセージを送信
	std::string inviteMsg = ":" + client->getFullIdentifier() + " INVITE " + targetNickname + " :" + channelName;
	sendToClient(targetClient->getFd(), inviteMsg);
}
