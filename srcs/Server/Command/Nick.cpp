#include "../../../includes/IRC.hpp"

void Server::broadcastNickChange(Client* client, std::string &msg)
{
	std::set<Client*> sentClients;

	std::set<std::string> channels = client->getChannels();
	for (std::set<std::string>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		std::string channelName = *it;
		Channel* channel = getChannel(channelName);
		if (channel)
		{
			// チャンネルのメンバーリストを取得
			std::set<Client*> members = channel->getMembers();
			
			// 各メンバーをループ
			for (std::set<Client*>::iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt)
			{
				Client* member = *memberIt;

				// このメンバーにまだメッセージを送信していないかチェック
				if (sentClients.find(member) == sentClients.end())
				{
					sendToClient(member->getFd(), msg);
					sentClients.insert(member);
				}
			}
		}
	}
}

bool Server::isValidNickname(const std::string& nick)
{
	// ニックネームの長さチェック
	if (nick.length() < 1 || nick.length() > 9)
		return false;
	// 最初の文字は英字または特殊文字
	if (!std::isalpha(nick[0]) && !strchr("[]\\`_{}|", nick[0]))
		return false;
	// 後続の文字は英数字または特殊文字
	for (size_t i = 1; i < nick.length(); ++i)
	{
		if (!std::isalnum(nick[i]) && !strchr("[]\\`_{}|-", nick[i]))
			return false;
	}
	return true;
}

void Server::handleNick(Client* client, const std::vector<std::string> &data)
{
	// パラメータの不足を確認
	if (data.size() < 2)
	{
		sendToClient(client->getFd(), getServerPrefix() + " 431 " + client->getNickname() + " :No nickname given");
		return;
	}

	std::string newNick = data[1];
	newNick.erase(newNick.find_last_not_of(" \r\n\t") + 1);

	// ニックネームの有効性チェック
	if (!isValidNickname(newNick))
	{
		sendToClient(client->getFd(), getServerPrefix() + " 432 " + client->getNickname() + " " + newNick + " :Erroneous nickname");
		return;
	}

	// 既存のニックネームかチェック
	Client* existingClient = getClientByNickname(newNick);
	if (existingClient && existingClient != client)
	{
		// ニックネームがすでに使用されている
		sendToClient(client->getFd(), getServerPrefix() + " 433 " + client->getNickname() + " " + newNick + " :Nickname is already in use");
		return;
	}

	// ニックネームの変更
	std::string oldNick = client->getNickname();
	client->setNickname(newNick);

	// ニックネーム変更の通知
	std::string nickMsg = "";
	if (oldNick.empty())
	{
		if (client->getUsername().empty())
			nickMsg = ":" + newNick + " NICK :" + newNick;
		else
			nickMsg = ":" + client->getFullIdentifier() + " NICK :" + newNick;
	}
	else
	{
		if (client->getUsername().empty())
			nickMsg = ":" + oldNick + " NICK :" + newNick;
		else
			nickMsg = ":" + oldNick + "!" + client->getUsername() + "@" + client->getHostname() + " NICK :" + newNick;
	}
	sendToClient(client->getFd(), nickMsg);
	broadcastNickChange(client, nickMsg);

	// 認証完了判定（NICK と USER 両方受けているか）
	if (!client->isRegistered()
	&& client->isAuthenticated()
	&& !client->getNickname().empty()
	&& !client->getUsername().empty())
	{
		client->setRegistered(true);
		sendWelcomeMessages(client);
		// getOrCreateChannel(client->getNickname()); もしかしたらつくかも？　irssi側ではnicknameのチャンネルを探していたため

	}
}
