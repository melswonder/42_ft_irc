#include "../../../includes/IRC.hpp"

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
	// 変更前と変更後の両方のユーザーに通知
	// :old_nick NICK :new_nick
	std::string nickMsg = ":" + oldNick + " NICK :" + newNick;
	sendToClient(client->getFd(), nickMsg);

	// ニックネーム変更によって認証が完了するかチェック
	if (!client->isRegistered() && client->isAuthenticated() && !client->getNickname().empty() && !client->getUsername().empty())
	{
		// 認証完了
		client->setRegistered(true);
		// RPL_WELCOME (001) を送信
		sendToClient(client->getFd(), getServerPrefix() + " 001 " + client->getNickname() + " :Welcome to the IRC Network " + client->getFullIdentifier());
	}
}
