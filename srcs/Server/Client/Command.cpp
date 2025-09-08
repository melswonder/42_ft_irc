#include "../../../includes/IRC.hpp"

// === INFO ===
void Server::serverInfo()
{
	std::cout << *this << std::endl;
}

// === EXIT === 
void Server::disconnectClient(int clientFd)
{
    for (size_t i = 0; i < _pollFds.size(); ++i)
    {
        if (_pollFds[i].fd == clientFd)
        {
            _pollFds.erase(_pollFds.begin() + i);
            std::map<int, Client *>::iterator it_client = _clients.find(clientFd);
            if (it_client != _clients.end())
            {
                delete it_client->second;
                _clients.erase(it_client);
            }
            break;
        }
    }
    close(clientFd);
}

// === PING ===
void Server::serverPing(int clientFd)
{
	std::string response = "PONG\n";
	send(clientFd, response.c_str(), response.length(), 0);
}

//パスワード認証が終わったらこのレスポンスを返さなければいけない
// Received: CAP LS
// NICK hirwatan
// USER hirwatan hirwatan localhost :Hiro Watanabe