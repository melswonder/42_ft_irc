#include "../../../includes/IRC.hpp"

// === INFO ===
void Server::serverInfo()
{
	std::cout << *this << std::endl;
}

// === EXIT ===
void Server::disconnectClient(int clientFd)
{
    std::map<int, Client *>::iterator it_client = _clients.find(clientFd);
    if (it_client != _clients.end())
    {
        Client *client = it_client->second;

        // クライアントが参加している全チャンネルから削除
        std::set<std::string> clientChannels = client->getChannels();
        for (std::set<std::string>::iterator ch_it = clientChannels.begin();
             ch_it != clientChannels.end(); ++ch_it)
        {
            Channel *channel = getChannel(*ch_it);
            if (channel)
            {
                channel->removeMember(client);

                // チャンネルが空になったら削除
                if (channel->isEmpty())
                {
                    std::map<std::string, Channel*>::iterator channel_it = _channels.find(*ch_it);
                    if (channel_it != _channels.end())
                    {
                        delete channel_it->second;
                        _channels.erase(channel_it);
                    }
                }
            }
        }

        delete client;
        _clients.erase(it_client);
    }

    // pollFdsから削除
    for (size_t i = 0; i < _pollFds.size(); ++i)
    {
        if (_pollFds[i].fd == clientFd)
        {
            _pollFds.erase(_pollFds.begin() + i);
            break;
        }
    }

    // クライアントバッファも削除
    _clientBuffers.erase(clientFd);

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
