#include "../../include/Channel.hpp"
#include "../../include/Client.hpp"
#include "../../include/Server.hpp"

int Server::set_nonblocking(int fd)
{
    int flags = fcntl(fd,F_GETFL,0);
    if(flags == -1){
        std::cerr << "fcntl () error:" << std::strerror(errno) << std::endl;
        return -1;
    }
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    return 0;
}
