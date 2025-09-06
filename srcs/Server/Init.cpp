#include "../../includes/IRC.hpp"

int checkValidPort(const char *port)
{
	int port_number = std::atoi(port);
	for (int i = 0; i < (int)strlen(port); i++)
	{
		if (std::isdigit(port[i]) == 0)
			throw std::runtime_error(SERVER_INVALID_PORT_CHARACTERS);
	}
	if (port_number < 1024 || port_number > 65535)
		throw std::runtime_error(SERVER_RESERVED_PORT_NUMBER);
	return port_number;
}

std::string checkValidPassword(const char *str)
{
	if (str == NULL || *str == '\0')
		throw std::runtime_error(SERVER_PASSWORD_EMPTY);

	std::string password = str;
	password.erase(password.find_last_not_of(" \r\n\t") + 1);
	password.erase(0, password.find_first_not_of(" \r\n\t"));
	return password;
}

void Server::serverInit(char *argv[])
{
	_port = checkValidPort(argv[1]);
	_password = checkValidPassword(argv[2]);
	_password = xorEncryptDecrypt(_password);
	this->setServerName("ircserv");

	_listeningSocketFd = socket(AF_INET, SOCK_STREAM, 0); // socket これはネットワーク通信のエンドポイント（端点）ソケットを作成する関数
	if (_listeningSocketFd == -1)
		throw std::runtime_error(SERVER_SOCKET_ERROR);

	int opt = 1;
	// 作成したソケットのオプションを設定する関数　SOL_SOCKETはソケット全般に関わるオプション SO_REUSEADDRはアドレスの再利用を許可するオプション
	if (setsockopt(_listeningSocketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "Error: setsockopt failed - " << strerror(errno) << std::endl;
		close(_listeningSocketFd);
		throw std::runtime_error("setsockopt failed");
	}

	// ノンブロッキングにする
	setNonblocking(_listeningSocketFd);
	// アドレス構造体を設定する
	setServerAddr(_port);

	if (bind(_listeningSocketFd, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) == -1)
	{
		std::cerr << "Error: Failed to bind to port " << _port << " - " << strerror(errno) << std::endl;
		close(_listeningSocketFd);
		throw std::runtime_error("Bind failed");
	}

	if (listen(_listeningSocketFd, 10) == -1)
	{
		std::cerr << "Error: Failed to listen - " << strerror(errno) << std::endl;
		close(_listeningSocketFd);
		throw std::runtime_error("Listen failed");
	}

	struct pollfd serverPollFd;
	serverPollFd.fd = _listeningSocketFd;
	serverPollFd.events = POLLIN;
	serverPollFd.revents = 0;
	_pollFds.push_back(serverPollFd);

	// std::cout << "=== Server Initialization Complete ===" << std::endl;
}
