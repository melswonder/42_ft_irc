#include "../../include/Server.hpp"

Server::Server() {}
Server::~Server() {}

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

std::ostream &operator<<(std::ostream &out, const Server &Server)
{
    out << "Port to bind: " << Server.getPort() << std::endl
        << "fd :" << Server.getListeningSocketFd() << std::endl;
    return out;
}
