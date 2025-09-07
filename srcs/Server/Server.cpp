#include "../../includes/IRC.hpp"

Server::Server() :_signal(false){}
Server::~Server()
{
	// チャンネルマップの解放
	std::map<std::string, Channel *>::iterator it_channel = _channels.begin();
	while (it_channel != _channels.end())
	{
		delete it_channel->second;
		it_channel++;
	}
	_channels.clear();

	// クライアントマップの解放
	std::map<int, Client *>::iterator it_client = _clients.begin();
	while (it_client != _clients.end())
	{
		delete it_client->second;
		it_client++;
	}
	_clients.clear();
}

void Server::serverRun()
{
	std::cout << "Server is running and listening on port " << _port << std::endl;
	int numEvents = 0;
	while (this->_signal == false)
	{
		numEvents = poll(&_pollFds[0], _pollFds.size(), -1);

		if (numEvents < 0)
		{
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
						i--; // 本来ユーザー側からEXITするコマンドの実装の意味があるのかは不明
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
	char buffer[512];
	int bytesRead;

	bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
	if (bytesRead <= 0)
	{
		disconnectClient(clientFd);
		return DISCONNECT;
	}
	buffer[bytesRead] = '\0';	 // readしたもんの末尾に\0をつけるようなもん。
	std::string message(buffer); // 渡された文字にコマンドがあるかチェックするため別のに入れる。

	message.erase(message.find_last_not_of(" \r\n\t") + 1); // 末尾の\r\nを消さないとifできない
	message.erase(0, message.find_first_not_of(" \r\n\t"));
	if (message.empty())
	{
		return CONNECT; // 空のメッセージは無視
	}

	std::map<int, Client *>::iterator it = _clients.find(clientFd);
	std::vector<std::string> data = split(message, '\n');

	for (size_t i = 0; i < data.size(); i++)
	{
		std::vector<std::string> split_data = split(data[i], ' ');
		std::string command = split_data[0];
		std::transform(command.begin(), command.end(), command.begin(), ::toupper);

		if (command == "CAP") // 拡張機能どれ使うかをirssi側からServerに送られる、今回は実装不可
			continue;
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
					handlePass(it->second, split_data);
				}
				// NICKとUSERコマンドのハンドリングロジックをここに追加
				else if (it->second->isAuthenticated())
				{
					if (command == "NICK")
						handleNick(it->second, split_data);
					else if (command == "USER")
						handleUser(it->second, split_data);
				}
				else
				{
					sendToClient(clientFd, getServerPrefix() + ERR_NEEDMOREPARAMS +"* PASS :Not enough parameters");
				}
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
				sendToClient(clientFd, getServerPrefix() + ERR_NOTREGISTERED + "* :You have not registered");
			}
		}
		else // 認証済みの場合
		{
			// 認証後のコマンドを処理
			if (command == "EXIT")
			{
				disconnectClient(clientFd);
				return DISCONNECT;
			}
			else if(command == "PASS")
				handlePass(it->second, split_data);
			else if(command == "NICK")
				handleNick(it->second, split_data);
			else if(command == "USER")
				handleUser(it->second, split_data);
			else if (command == "INFO")
				serverInfo();
			else if (command == "END")
				throw std::runtime_error("END!");
			else if (command == "PING")
				serverPing(clientFd);
			else if (command == "JOIN")
				handleJoin(it->second, split_data);
			else if (command == "PRIVMSG")
				handlePrivmsg(it->second, split_data);
			else if (command == "KICK")
				handleKick(it->second, split_data);
			else if (command == "MODE")
				handleMode(it->second, split_data);
			else if (command == "INVITE")
				handleInvite(it->second, split_data);
			else if (command == "TOPIC")
				handleTopic(it->second, split_data);
			else
			{
				std::cout << "Unknown command: " << command << std::endl;
				sendToClient(clientFd, getServerPrefix() + ERR_UNKNOWNCOMMAND + "* " + command + " :Unknown command");
			}
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

void Server::setServerName(std::string serverName)
{
	if (!serverName.empty())
		this->_serverName = serverName;
}

void Server::setClientAuthentications(int newfd)
{
	Client *newClient = new Client(newfd);
	_clients.insert(std::make_pair(newfd, newClient));
	delete newClient;
	std::map<int, Client *>::iterator it = _clients.find(newfd);
	it->second->setAuthenticated(false);
	std::cout << "Created new client for fd: " << newfd << std::endl;
}

void Server::setServerAddr(int port_number)
{
	memset(&_server_addr, 0, sizeof(_server_addr));
	_server_addr.sin_family = AF_INET;			// IPv4の使用
	_server_addr.sin_addr.s_addr = INADDR_ANY;	// どのIPからも接続を許可
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

std::map<int, Client *> Server::getClientAuthentications(void) const
{
	return (this->_clients);
}

Client *Server::getClient(int fd)
{
	std::cout << "getClient()" << std::endl;
	std::map<int, Client *>::iterator it = _clients.find(fd);
	if (it != _clients.end())
		return (it->second);
	return NULL;
}

Channel *Server::getChannel(const std::string &channelName)
{
	std::map<std::string, Channel *>::iterator it = _channels.find(channelName);
	if (it != _channels.end())
		return it->second;
	return NULL;
}

Client *Server::getClientByNickname(const std::string &nickname)
{
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second->getNickname() == nickname)
			return it->second;
	}
	return NULL;
}

std::string Server::getServerPrefix() const
{
	return ":" + _serverName;
}

std::ostream &operator<<(std::ostream &out, const Server &server)
{
	out << "Port to bind: " << server.getPort() << std::endl
		<< "Password :" << server.getPassword() << std::endl
		<< "Serverfd :" << server.getListeningSocketFd() << std::endl;
	const std::map<int, Client *> &authentications = server.getClientAuthentications();
	for (std::map<int, Client *>::const_iterator it = authentications.begin();
		 it != authentications.end(); it++)
		out << "fd[" << it->first << "] 認証 :" << (it->second->isAuthenticated() ? "有効" : "無効") << std::endl;
	return out;
}

void Server::sendWelcomeMessages(Client *client)
{
	std::string serverName = _serverName;
	std::string nick = client->getNickname();
	std::string userHost = nick + "!" + client->getUsername() + "@" + client->getHostname();

	// 001
	sendToClient(client->getFd(), ":" + serverName + RPL_WELCOME + nick + " :Welcome to the ft_irc network, " + userHost);
	// 002
	sendToClient(client->getFd(), ":" + serverName + RPL_YOURHOST + nick + " :Your host is " + serverName + ", running version ft_irc-1.0");
	// 003
	time_t now = time(NULL);
	std::string createdTime = ctime(&now);
	createdTime.erase(createdTime.find_last_not_of(" \n\r\t") + 1);
	sendToClient(client->getFd(), ":" + serverName + RPL_CREATED + nick + " :This server was created " + createdTime);
	// 004
	sendToClient(client->getFd(), ":" + serverName + RPL_MYINFO + nick + " :" + serverName + " ft_irc-1.0 i t k o l");
	// 005
	sendToClient(client->getFd(), ":" + serverName + RPL_ISUPPORT + nick + " :CHANMODES=i,t,k,o,l PREFIX=(o)@ CHANTYPES=# :are supported by this server");
	// MOTD
	sendToClient(client->getFd(), ":" + serverName + RPL_MOTDSTART + nick + " :- " + serverName + " Message of the day -");
	sendToClient(client->getFd(), ":" + serverName + RPL_MOTD + nick + " :- Welcome to the ft_irc server!");
	sendToClient(client->getFd(), ":" + serverName + RPL_MOTD + nick + " :- Please follow the rules.");
	sendToClient(client->getFd(), ":" + serverName + RPL_ENDOFMOTD + nick + " :End of MOTD command");
}
