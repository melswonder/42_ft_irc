#include "../../../include/Server.hpp"
#include "../../../include/Client.hpp"

void Server::checkAuthentication(std::string message, int clientFd)
{
	size_t space_pos = message.find(' ');
	std::string password = message.substr(space_pos + 1);
	password.erase(password.find_last_not_of(" \r\n\t") + 1);
	password.erase(0, password.find_first_not_of(" \r\n\t"));
	password = xorEncryptDecrypt(password);
	// std::cout << "Serverpass[" << _password << "]len:" << _password.length() << std::endl
	// 		  << "Clientpass[" << password << "]len:" << _password.length() << std::endl;
	if (_password == password)
	{
		std::cout << "認証OK!" << std::endl;
		std::map<int, Client>::iterator it = _client.find(clientFd);
		if (it != _client.end())
		{
			std::cout << "Password verified!" << std::endl;
			it->second.setAuthenticated(true);
		}
	}
	else
	{
		std::cout << "認証KO" << std::endl;
	}
}

void Server::serverInfo()
{
	std::cout << *this << std::endl;
}

// データが渡されたときの処理
void Server::handleClientData(int clientFd)
{
	char buffer[512];
	int bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);

	if (bytesRead <= 0)
	{
		disconnectClient(clientFd);
		return;
	}
	buffer[bytesRead] = '\0';	 // readしたもんの末尾に\0をつけるようなもん。
	std::string message(buffer); // 渡された文字にコマンドがあるかチェックするため別のに入れる。

	std::cout << GRE << message << WHI << std::endl;

    if (message.find("PASS") != std::string::npos)
        checkAuthentication(message, clientFd);
    
    Client client = getClient(clientFd);

    if (client.isAuthenticated() == true)
    {
		if (message.find("EXIT") != std::string::npos)
			disconnectClient(clientFd);
		else if (message.find("INFO") != std::string::npos)
			serverInfo();
		else if (message.find("END") != std::string::npos)
			throw std::runtime_error("END!");
		else if (message.rfind("PING", 0) == 0)
		{
			// PINGコマンドのパラメータ部分を抽出
			std::string parameter = message.substr(message.find(" ") + 1);

			// PONGにそのままパラメータを含めて応答
			std::string response = "PONG " + parameter;

			send(clientFd, response.c_str(), response.length(), 0);
		}
	}
	else
	{
		std::cout << "You are not authorized!!!" << std::endl;
	}

	// std::cout << "Received: " << buffer << std::flush; // サーバー側が出力

	// 単純なエコー応答（IRC形式）このsendがあると送ってくれた相手に送り返せる
	// std::string response = ":server PRIVMSG client :" + std::string(buffer) + "\r\n";
	// send(clientFd, response.c_str(), response.length(), 0);
}
