#include "../../includes/IRC.hpp"

Server::Server() {}
Server::~Server()
{
	// チャンネルマップの解放
    std::map<std::string, Channel*>::iterator it_channel = _channels.begin();
    while (it_channel != _channels.end()) {
        delete it_channel->second;
        it_channel++;
    }
	_channels.clear();

    // クライアントマップの解放
    std::map<int, Client*>::iterator it_client = _clients.begin();
    while (it_client != _clients.end()) {
        delete it_client->second;
        it_client++;
    }
	_clients.clear();
}

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
	if (message.empty()) {
        return CONNECT; // 空のメッセージは無視
    }

	// std::cout << GRE << message << WHI << std::endl;
	std::map<int, Client*>::iterator it = _clients.find(clientFd);

	// if (it == _clients.end())
	// {
	// 	Client newClient(clientFd);
	// 	_clients[clientFd] = newClient;
	// 	it = _clients.find(clientFd);
	// 	std::cout << "Created new Client for fd: " << clientFd << std::endl;
	// }

	std::vector<std::string> data = split(message, ' '); // コマンドを
	std::string command = data[0]; // コマンドの最初の部分を取得
	std::transform(command.begin(), command.end(), command.begin(), ::toupper);

	// 認証状態のチェック
	if (it->second->isRegistered() == false)
	{
		// 未認証の場合、許可されたコマンドのみを処理
		if (command == "PASS" || command == "NICK" || command == "USER")
		{
			// ここで認証関連の関数を呼び出す
			if (command == "PASS")
			{
				if (it->second->isAuthenticated())
					return CONNECT;
				handlePass(it->second, data);
			}
			// NICKとUSERコマンドのハンドリングロジックをここに追加
			else if (command == "NICK")
				handleNick(it->second, data);
			else if (command == "USER")
				handleUser(it->second, data);
		}
		else
		{
			// 許可されていないコマンドの場合、ログに出力し、エラーメッセージを送信
			std::stringstream ss;
			ss << it->second->getPort();
			std::string port_str = ss.str();
			
			std::cout << "Unauthorized command from unregistered client " 
						<< it->second->getHostname() 
						<< ":" 
						<< port_str 
						<< " \"" 
						<< command 
						<< "\"" << std::endl;
			
			// クライアントにエラーメッセージを送信
			sendToClient(clientFd, getServerPrefix() + " 451 * :You have not registered");
		}
	}
	else // 認証済みの場合
	{
		// 認証後のコマンドを処理
		if (command == "EXIT")
			disconnectClient(clientFd);
		else if (command == "INFO")
			serverInfo();
		else if (command == "END")
			throw std::runtime_error("END!");
		else if (command == "PING")
			serverPing(clientFd);
		else if (command == "NICK")
			handleNick(it->second, data);
		else if (command == "JOIN")
			handleJoin(it->second, data);
		else if (command == "PRIVMSG")
			handlePrivmsg(it->second, data);
		else if (command == "KICK")
			handleKick(it->second, data);
		else if (command == "MODE")
			handleMode(it->second, data);
		else if (command == "INVITE")
			handleInvite(it->second, data);
		else
		{
			std::cout << "Unknown command: " << command << std::endl;
			sendToClient(clientFd, getServerPrefix() + " 421 * " + command + " :Unknown command");
		}
	}
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
	Client *newClient = new Client(newfd);
	_clients.insert(std::make_pair(newfd, newClient));
	std::map<int, Client*>::iterator it = _clients.find(newfd);
	it->second->setAuthenticated(false);
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

std::map<int, Client*> Server::getClientAuthentications(void) const
{
	return (this->_clients);
}

Client* Server::getClient(int fd)
{
	std::cout << "getClient()" << std::endl;
	std::map<int, Client*>::iterator it = _clients.find(fd);
	if (it != _clients.end())
		return (it->second);
	return NULL;
}

Channel* Server::getChannel(const std::string& channelName)
{
	std::map<std::string, Channel*>::iterator it = _channels.find(channelName);
	if (it != _channels.end())
		return it->second;
	return NULL;
}

Client* Server::getClientByNickname(const std::string& nickname)
{
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second->getNickname() == nickname)
			return it->second;
	}
	return NULL;
}

std::string Server::getServerPrefix() const
{
	return ":" + std::string("ircserv");
}

std::ostream &operator<<(std::ostream &out, const Server &server)
{
	out << "Port to bind: " << server.getPort() << std::endl
		<< "Password :" << server.getPassword() << std::endl
		<< "Serverfd :" << server.getListeningSocketFd() << std::endl;
	const std::map<int, Client*> &authentications = server.getClientAuthentications();
	for (std::map<int, Client*>::const_iterator it = authentications.begin();
		it != authentications.end(); it++)
		out << "fd[" << it->first << "] 認証 :" << (it->second->isAuthenticated() ? "有効" : "無効") << std::endl;
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
