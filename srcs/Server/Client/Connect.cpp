#include "../../../include/IRC.hpp"

// 新しい接続があったとき、STLのfdをpushbackして追加する。
void Server::handleNewConnection()
{
	sockaddr_in clientAddr;
	socklen_t addrLen = sizeof(clientAddr);
	int newFd = accept(_listeningSocketFd, (struct sockaddr *)&clientAddr, &addrLen);
	if (newFd < 0)
	{
		std::cerr << "accept() error: " << std::strerror(errno) << std::endl;
		return;
	}

	if (setNonblocking(newFd) == -1)
		return;
	struct pollfd newPollFd;
	newPollFd.fd = newFd;
	newPollFd.events = POLLIN;
	_pollFds.push_back(newPollFd);
	setClientAuthentications(newFd); // 認証
	std::cout << RED << "New connection Fd:" << newPollFd.fd << " !" << WHI << std::endl;
}

void Server::disconnectClient(int clientFd)
{
	for (size_t i = 0; i < _pollFds.size(); ++i)
	{
		if (_pollFds[i].fd == clientFd)
		{
			_pollFds.erase(_pollFds.begin() + i);
			break;
		}
	}
	close(clientFd);
}

// ここから下は仮　コマンド実装なら以下を参考にするべき。
//  void Server::handleClientData(int clientFd) {
//      char buffer[1024];
//      int bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);

//     if (bytesRead <= 0) {
//         disconnectClient(clientFd);
//         return;
//     }

//     buffer[bytesRead] = '\0';
//     std::string message(buffer);

//     std::cout << "Received: " << message << std::endl;

//     // IRCコマンドの解析と応答
//     if (message.find("NICK") == 0) {
//         // ニックネーム設定の応答
//         std::string response = ":server 001 " + extractNick(message) + " :Welcome to the IRC Network\r\n";
//         send(clientFd, response.c_str(), response.length(), 0);
//     }
//     else if (message.find("USER") == 0) {
//         // ユーザー登録の応答（通常NICKと組み合わせて使用）
//         // 001応答は既にNICKで送信済み
//     }
//     else if (message.find("PRIVMSG") == 0) {
//         // プライベートメッセージのエコー
//         std::string response = ":" + extractSender(message) + " " + message + "\r\n";
//         send(clientFd, response.c_str(), response.length(), 0);
//     }
//     else if (message.find("PING") == 0) {
//         // PING応答
//         std::string response = "PONG " + message.substr(5) + "\r\n";
//         send(clientFd, response.c_str(), response.length(), 0);
//     }
// }

// std::string extractNick(const std::string& nickCommand) {
//     size_t spacePos = nickCommand.find(' ');
//     if (spacePos != std::string::npos) {
//         return nickCommand.substr(spacePos + 1);
//     }
//     return "unknown";
// }

// Received: CAP LS
// NICK hirwatan
// USER hirwatan hirwatan localhost :Hiro Watanabe
