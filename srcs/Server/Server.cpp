#include "../../include/IRC.hpp"

Server::Server() {}
Server::~Server() {}

void Server::serverRun()
{
	std::cout << "Server is running and listening on port " << _port << std::endl;
	int numEvents = 0;
	while (true)
	{
		numEvents = poll(&_pollFds[0], _pollFds.size(), -1);

		if (numEvents < 0)
		{
			if (errno == EINTR)
				continue;
			std::cerr << "poll() error" << std::endl;
			break;
		}

		// numEventsが0の場合(タイムアウト時)の処理を省略
		for (size_t i = 0; i < _pollFds.size(); ++i)
		{
			if (_pollFds[i].revents == 0)
				continue;

			// 接続街のソケットからのイベント
			if (_pollFds[i].fd == _listeningSocketFd)
			{
				if (_pollFds[i].revents & POLLIN)
					handleNewConnection();
			}
			else
			{
				if (_pollFds[i].revents & POLLIN)
				{
					if (handleClientData(_pollFds[i].fd) == DISCONNECT)
						i--; //本来ユーザー側からEXITするコマンドの実装の意味があるのかは不明
				}
				else if (_pollFds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
				{
					std::cerr << "Client disconnected unexpectedly" << std::endl;
					disconnectClient(_pollFds[i].fd);
					i--;
				}
			}
		}
	}
}

// 書くfdの処理があった時
Situation Server::handleClientData(int clientFd)
{
	char	buffer[512];
	int		bytesRead;

	bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
	if (bytesRead <= 0)
	{
		disconnectClient(clientFd);
		return DISCONNECT;
	}
	buffer[bytesRead] = '\0';    // readしたもんの末尾に\0をつけるようなもん。
	std::string message(buffer); // 渡された文字にコマンドがあるかチェックするため別のに入れる。

	message.erase(message.find_last_not_of(" \r\n\t") + 1); //末尾の\r\nを消さないとifできない
	message.erase(0, message.find_first_not_of(" \r\n\t"));

	std::cout << GRE << message << WHI << std::endl;
	std::map<int, Client>::iterator it = _client.find(clientFd);

	if (it == _client.end())
	{
		Client newClient(clientFd);
		_client[clientFd] = newClient;
		it = _client.find(clientFd);
		std::cout << "Created new Client for fd: " << clientFd << std::endl;
	}

	Client& client = it->second;
	std::vector<std::string> data = split(message, '\n'); // コマンドを

	if (data.empty()) // check args for any command
		return CONNECT;
	for (size_t i = 0; i < data.size(); ++i)
	{
		std::vector<std::string> split_data = split(data[i], ' ');

		if (split_data[0] == "CAP")
			continue;
		if (split_data[0] == "PASS" && split_data.size() == 2)
			checkAuthentication(split_data[1], clientFd);
		if (split_data[0] == "INFO")
			serverInfo();
		if (client.isAuthenticated() == true)
		{
			if (split_data[0] == "EXIT")
				disconnectClient(clientFd);
			else if (split_data[0] == "END") // サーバー側のメモリリークチェックよう
				throw std::runtime_error("END!");
			else if (split_data[0] == "PING")
				serverPing(clientFd);
			else if (split_data.size() >= 2)
			{
				if (split_data[0] == "NICK" && !split_data[1].empty())
					client.setNewNickname(clientFd, split_data[1]);
				else if (split_data[0] == "USER" && !split_data[1].empty())
					client.setNewUsername(clientFd, split_data[1]);
			}
		}
		else
		{
			// std::string response = "You are not authorized!!!";
			// send(clientFd, response.c_str(), response.length(), 0);　//ユーザーに見せる場合

			std::cout << "You are not authorized!!!" << std::endl; //サーバー側の出力
		}
	}

	// std::cout << "Received: " << buffer << std::flush; // サーバー側が出力
	// 単純なエコー応答（IRC形式）このsendがあると送ってくれた相手に送り返せる
	// std::string response = ":server PRIVMSG client :" + std::string(buffer)
		// + "\r\n";
	// send(clientFd, response.c_str(), response.length(), 0);
	return CONNECT;
}

//===setter===
void Server::setPort(int port)
{
	this->_port = port;
}

void Server::setPassword(std::string password)
{
	this->_password = password;
}

void Server::setListeningSocketFd(int listeningSocketFd)
{
	this->_listeningSocketFd = listeningSocketFd;
}

void Server::setClientAuthentications(int newfd)
{
	Client newClient(newfd);
	_client.insert(std::make_pair(newfd, newClient));
	std::map<int, Client>::iterator it = _client.find(newfd);
	it->second.setAuthenticated(false);
	std::cout << "Created new client for fd: " << newfd << std::endl;
}

// この関数はヘルパー関数です　後にbind()という関数で、
// ソケット通信に必要な情報を整理し、関数に渡すための手段を踏んでいます
void Server::setServerAddr(int port_number)
{
	memset(&_server_addr, 0, sizeof(_server_addr));
	_server_addr.sin_family = AF_INET;          // IPv4の使用
	_server_addr.sin_addr.s_addr = INADDR_ANY;  // どのIPからも接続を許可
	_server_addr.sin_port = htons(port_number); // ポート番号設定
}

//===getter===
int Server::getPort(void) const
{
	return (this->_port);
}

std::string Server::getPassword(void) const
{
	return (this->_password);
}

int Server::getListeningSocketFd(void) const
{
	return (this->_listeningSocketFd);
}

std::map<int, Client> Server::getClientAuthentications(void) const
{
	return (this->_client);
}

Client* Server::getClient(int fd)
{
	std::cout << "getClient()" << std::endl;
	std::map<int, Client>::iterator it = _client.find(fd);
	if (it != _client.end())
		return &(it->second);
	return NULL;
}

std::ostream &operator<<(std::ostream &out, const Server &server)
{
	out << "Port to bind: " << server.getPort() << std::endl
		<< "Password :" << server.getPassword() << std::endl
		<< "Serverfd :" << server.getListeningSocketFd() << std::endl;
	const std::map<int, Client> &authentications = server.getClientAuthentications();
	for (std::map<int, Client>::const_iterator it = authentications.begin();
		it != authentications.end(); it++)
		out << "fd[" << it->first << "] 認証 :" << (it->second.isAuthenticated() ? "有効" : "無効") << std::endl;
	return out;
}

// void sendNumericReply(int clientFd, const std::string& serverName, const std::string& clientNick, const std::string& code, const std::string& message) {
//     std::stringstream ss;
//     ss << ":" << serverName << " " << code << " " << clientNick << " :" << message << "\r\n";
//     std::string response = ss.str();
//     send(clientFd, response.c_str(), response.length(), 0);
// }

// void sendAllWelcomeReplies(int clientFd, const std::string& serverName, const std::string& clientNick) {
//     // RPL_WELCOME 001
//     sendNumericReply(clientFd, serverName, clientNick, "001", "Welcome to the ft_irc network, " + clientNick + "!");
    
//     // RPL_YOURHOST 002
//     sendNumericReply(clientFd, serverName, clientNick, "002", "Your host is " + serverName + ", running version ft_irc-1.0");

//     // RPL_CREATED 003
//     time_t now = time(0);
//     std::string createdTime = ctime(&now);
//     sendNumericReply(clientFd, serverName, clientNick, "003", "This server was created " + createdTime);

//     // RPL_MYINFO 004
//     sendNumericReply(clientFd, serverName, clientNick, "004", serverName + " ft_irc-1.0 i t k o l");
    
//     // MOTD
//     sendNumericReply(clientFd, serverName, clientNick, "375", "- " + serverName + " Message of the day -");
//     sendNumericReply(clientFd, serverName, clientNick, "372", "- Welcome to the ft_irc server!");
//     sendNumericReply(clientFd, serverName, clientNick, "372", "- Please follow the rules.");
//     sendNumericReply(clientFd, serverName, clientNick, "376", "End of MOTD command");
// }
