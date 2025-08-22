#include "../../../includes/IRC.hpp"

void Server::executeSingleKickLogic(Server* server, Client* kicker, Channel* channel, Client* target, const std::string& comment)
{
	// チャンネルからターゲットを削除
	channel->removeMember(target);

	// KICKメッセージの構築
	std::string kickMsg = ":" + kicker->getFullIdentifier() + " KICK " + channel->getName() + " " + target->getNickname();
	if (!comment.empty())
		kickMsg += " :" + comment;

	// チャンネルメンバーとターゲットに通知
	server->broadcastToChannel(channel->getName(), kickMsg); // チャンネル全員にキックメッセージをブロードキャスト
	server->sendToClient(target->getFd(), kickMsg); // キックされたユーザーにも通知

	// チャンネルが空になったら削除する
	if (channel->isEmpty())
	{
		// マップからチャンネルを見つけて削除
		std::map<std::string, Channel*>::iterator it = server->_channels.find(channel->getName());
		if (it != server->_channels.end())
		{
			delete it->second; // チャンネルオブジェクトを削除
			server->_channels.erase(it); // マップからエントリを削除
		}
	}
}

void Server::handleKick(Client* client, const std::vector<std::string> &data)
{
	if (data.size() < 3)
	{
		sendToClient(client->getFd(), getServerPrefix() + " 461 " + client->getNickname() + " KICK :Not enough parameters");
		return;
	}

	// コメント部分
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
			// 単一の単語の場合
			if (data.size() == 4)
				comment = data[3];
			else
			{
				sendToClient(client->getFd(), getServerPrefix() + " 461 " + client->getNickname() + " KICK :Invalid comment format or too many parameters");
				return;
			}
		}
	}

	// チャンネル名とターゲットニックネームのリストをパース
	std::vector<std::string> channelNames = split(data[1], ',');
	std::vector<std::string> targetNicks = split(data[2], ',');

	// RFC 2812のKICKコマンドの複数パラメータのルールチェック
	if (!((channelNames.size() == 1 && targetNicks.size() >= 1) || (channelNames.size() > 1 && channelNames.size() == targetNicks.size())))
	{
		sendToClient(client->getFd(), getServerPrefix() + " 461 " + client->getNickname() + " KICK :Invalid channel/user combination for KICK command");
		return;
	}

	// 単一チャンネル
	if (channelNames.size() == 1)
	{
		std::string currentChannelName = channelNames[0]; // 単一のチャンネル名を取得

		// チャンネルの存在チェック: ERR_NOSUCHCHANNEL (403)
		std::map<std::string, Channel*>::iterator channelIt = _channels.find(currentChannelName);
		if (channelIt == _channels.end())
		{
			sendToClient(client->getFd(), getServerPrefix() + " 403 " + client->getNickname() + " " + currentChannelName + " :No such channel");
			return; // このチャンネルが存在しないなら、このKICKコマンド全体を中断
		}
		Channel* channel = channelIt->second;

		// 実行者がチャンネルメンバーかチェック: ERR_NOTONCHANNEL (442)
		if (!channel->isMember(client))
		{
			sendToClient(client->getFd(), getServerPrefix() + " 442 " + client->getNickname() + " " + currentChannelName + " :You're not on that channel");
			return;
		}

		// 実行者がチャンネルオペレーターかチェック: ERR_CHANOPRIVSNEEDED (482)
		if (!channel->isOperator(client))
		{
			sendToClient(client->getFd(), getServerPrefix() + " 482 " + client->getNickname() + " " + currentChannelName + " :You're not channel operator");
			return;
		}

		// 全てのターゲットユーザーを処理
		for (size_t i = 0; i < targetNicks.size(); ++i)
		{
			std::string targetNick = targetNicks[i];
			Client* targetClient = getClientByNickname(targetNick);
			if (!targetClient) // ネットワーク上に存在しない
			{
				sendToClient(client->getFd(), getServerPrefix() + " 401 " + client->getNickname() + " " + targetNick + " :No such nick/channel");
				continue; // 次のユーザーへ (このユーザーのキックは失敗)
			}
			if (!channel->isMember(targetClient)) // チャンネル内にいない
			{
				sendToClient(client->getFd(), getServerPrefix() + " 441 " + client->getNickname() + " " + targetNick + " " + currentChannelName + " :They aren't on that channel");
				continue; // 次のユーザーへ (このユーザーのキックは失敗)
			}
			
			// KICKの実行
			executeSingleKickLogic(this, client, channel, targetClient, comment);
		}
	}
	// 複数チャンネル
	else
	{
		for (size_t i = 0; i < channelNames.size(); ++i)
		{
			std::string currentChannelName = channelNames[i];
			std::string currentTargetNick = targetNicks[i];

			// チャンネルの存在チェック: ERR_NOSUCHCHANNEL (403)
			std::map<std::string, Channel*>::iterator channelIt = _channels.find(currentChannelName);
			if (channelIt == _channels.end())
			{
				sendToClient(client->getFd(), getServerPrefix() + " 403 " + client->getNickname() + " " + currentChannelName + " :No such channel");
				continue; // 次のチャンネル/ユーザーペアへ
			}
			Channel* channel = channelIt->second;

			// 実行者がチャンネルメンバーかチェック: ERR_NOTONCHANNEL (442)
			if (!channel->isMember(client))
			{
				sendToClient(client->getFd(), getServerPrefix() + " 442 " + client->getNickname() + " " + currentChannelName + " :You're not on that channel");
				continue;
			}

			// 実行者がチャンネルオペレーターかチェック: ERR_CHANOPRIVSNEEDED (482)
			if (!channel->isOperator(client))
			{
				sendToClient(client->getFd(), getServerPrefix() + " 482 " + client->getNickname() + " " + currentChannelName + " :You're not channel operator");
				continue;
			}

			// ターゲットユーザーの存在チェック: ERR_NOSUCHNICK (401)
			// ターゲットがチャンネル内にいるかどうかも確認
			Client* targetClient = getClientByNickname(currentTargetNick);
			if (!targetClient) // ネットワーク上に存在しない
			{ 
				sendToClient(client->getFd(), getServerPrefix() + " 401 " + client->getNickname() + " " + currentTargetNick + " :No such nick/channel");
				continue;
			}
			if (!channel->isMember(targetClient)) // チャンネル内にいない
			{
				sendToClient(client->getFd(), getServerPrefix() + " 441 " + client->getNickname() + " " + currentTargetNick + " " + currentChannelName + " :They aren't on that channel");
				continue;
			}

			// KICKの実行
			executeSingleKickLogic(this, client, channel, targetClient, comment);
		}
	}
}
