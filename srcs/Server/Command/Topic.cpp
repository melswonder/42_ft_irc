#include "IRC.hpp"

void Server::handleTopic(Client* client, const std::vector<std::string> &data) {
	// パラメータのチェック: ERR_NEEDMOREPARAMS (461)
	if (data.size() < 2) {
		sendToClient(client->getFd(), getServerPrefix() + ERR_NEEDMOREPARAMS + client->getNickname() + " TOPIC :Not enough parameters");
		return;
	}

	std::string channelName = data[1];
	Channel* channel = getChannel(channelName);

	// チャンネルの存在チェック: ERR_NOSUCHCHANNEL (403)
	if (!channel) {
		sendToClient(client->getFd(), getServerPrefix() + ERR_NOSUCHCHANNEL + client->getNickname() + " " + channelName + " :No such channel");
		return;
	}

	// クライアントがチャンネルにいるか確認: ERR_NOTONCHANNEL (442)
	if (!channel->isMember(client)) {
		sendToClient(client->getFd(), getServerPrefix() + ERR_NOTONCHANNEL + client->getNickname() + " " + channelName + " :You're not on that channel");
		return;
	}

	// トピックの表示
	if (data.size() == 2) {
		if (channel->getTopic().empty()) {
			// RPL_NOTOPIC (331) を送信: トピックが設定されていない
			sendToClient(client->getFd(), getServerPrefix() + RPL_NOTOPIC + client->getNickname() + " " + channelName + " :No topic is set");
		} else {
			// RPL_TOPIC (332) を送信: トピックを表示
			sendToClient(client->getFd(), getServerPrefix() + RPL_TOPIC + client->getNickname() + " " + channelName + " :" + channel->getTopic());
		}
		return;
	}

	// トピックの変更

	// チャンネルモードの確認: ERR_CHANOPRIVSNEEDED (482)
	// チャンネルモードが +t で、クライアントがオペレーターでない場合
	if (channel->isTopicRestricted() && !channel->isOperator(client)) {
		sendToClient(client->getFd(), getServerPrefix() + ERR_CHANOPRIVSNEEDED + client->getNickname() + " " + channelName + " :You're not channel operator");
		return;
	}

	// 新しいトピックの取得
	std::string newTopic;
	if (data[2].length() > 0 && data[2][0] == ':') {
		newTopic = data[2].substr(1);
		for (size_t i = 3; i < data.size(); ++i) {
			newTopic += " " + data[i];
		}
	} else {
		newTopic = data[2];
	}

	// トピックの設定
	channel->setTopic(newTopic);

	// チャンネルメンバーにトピック変更をブロードキャスト
	// :<client> TOPIC <channel> :<new_topic>
	std::string topicMsg = ":" + client->getFullIdentifier() + " TOPIC " + channelName + " :" + newTopic;
	broadcastToChannel(channelName, topicMsg);
}
