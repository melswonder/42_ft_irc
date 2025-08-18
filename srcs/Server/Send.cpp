#include "IRC.hpp"

void Server::sendToClient(int clientFd, const std::string& message)
{
	std::string fullMessage = message + "\r\n";
	send(clientFd, fullMessage.c_str(), fullMessage.length(), 0);
}

void Server::broadcastToChannel(const std::string& channelName, const std::string& message, Client* sender) {
    std::map<std::string, Channel*>::iterator channelIt = _channels.find(channelName);
    if (channelIt == _channels.end()) {
        return;
    }
    
    Channel* channel = channelIt->second;
    const std::set<Client*>& members = channel->getMembers();
    
    for (std::set<Client*>::const_iterator it = members.begin(); it != members.end(); ++it) {
        if (*it != sender) {
            sendToClient((*it)->getFd(), message);
        }
    }
}
