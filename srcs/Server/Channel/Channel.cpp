#include "IRC.hpp"

Channel* Server::getOrCreateChannel(const std::string& channelName) {
    std::map<std::string, Channel*>::iterator it = _channels.find(channelName);
    if (it != _channels.end()) {
        return it->second;
    }
    
    Channel* newChannel = new Channel(channelName);
    _channels[channelName] = newChannel;
    return newChannel;
}
