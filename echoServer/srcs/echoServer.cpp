#include "../include/echoServer.hpp"

echoServer::echoServer(){};
echoServer::~echoServer(){};

void echoServer::disconnectClient(int clientFd) {
    for (size_t i = 0; i < _pollFds.size(); ++i) {
        if(_pollFds[i].fd == clientFd) {
            _pollFds.erase(_pollFds.begin() + i);
            break;
        }
    }
    close(clientFd);
}

std::ostream &operator<<(std::ostream &out, const echoServer &Server)
{
    out << "Port to bind: " << Server.getPort()  << std::endl
        << "fd :" << Server.getListeningSocketFd() << std::endl;
    return out;
}
