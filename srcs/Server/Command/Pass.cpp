#include "../../../includes/IRC.hpp"

//=== PASS ===
void Server::handlePass(Client* client, const std::vector<std::string> &data)
{
	std::string password = data[1];
	password.erase(password.find_last_not_of(" \r\n\t") + 1);
	password.erase(0, password.find_first_not_of(" \r\n\t"));
	password = xorEncryptDecrypt(password);
	// std::cout << "Serverpass[" << _password << "]len:" << _password.length() << std::endl
	// 			<< "Clientpass[" << password << "]len:" << _password.length() << std::endl;
	if (_password == password)
	{
		std::cout << "Authentication successful for client " << client->getFd() << "." << std::endl;
		client->setAuthenticated(true);
	}
	else
	{
		// 認証失敗
		std::cout << "Authentication failed for client " << client->getFd() << ". Incorrect password." << std::endl;
		
		// エラーメッセージをクライアントに送信
		std::string fail_msg = getServerPrefix() + " 464 * :Password incorrect";
		sendToClient(client->getFd(), fail_msg.c_str());
	}
	// ニックネーム変更によって認証が完了するかチェック
	if (!client->isRegistered() && client->isAuthenticated() && !client->getNickname().empty() && !client->getUsername().empty())
	{
		// 認証完了
		client->setRegistered(true);
		// // RPL_WELCOME (001) を送信
		// sendToClient(client->getFd(), getServerPrefix() + " 001 " + client->getNickname() + " :Welcome to the IRC Network " + client->getFullIdentifier());
	}
}
