#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h> //-> for socket()
#include <sys/types.h>  //-> for socket()
#include <netinet/in.h> //-> for sockaddr_in
#include <fcntl.h>      //-> for fcntl()
#include <unistd.h>     //-> for close()
#include <arpa/inet.h>  //-> for inet_ntoa()
#include <poll.h>       //-> for poll()
#include <csignal>      //-> for signal()
#include <signal.h>     // signal関数を使うために必要
#include <netdb.h>      // getaddrinfoを使う
#include <string.h>
#include <cstring>  // memset()
#include <cerrno>   // errno
#include <stdlib.h> // atoi


#define RED "\033[1;31m" //-> for red color
#define WHI "\033[0;37m" //-> for white color
#define GRE "\033[1;32m" //-> for green color
#define YEL "\033[1;33m" //-> for yellow color

class Server
{
private:
    // bool _signal;
    int _listeningSocketFd;
    std::string _port;
    std::vector<struct pollfd> _pollFds;
    struct sockaddr_in _server_addr;

    void handleNewConnection();
    void handleClientData(int clientFd);
    void disconnectClient(int clientFd);
public:
    Server();
    ~Server();
    void ServerInit(char *argv[]);
    void ServerRun(void);

    void initAddrinfo(in_port_t sin_port,struct in_addr sin_addr);
    
    //setter
    void setAddr(int port_nuber);


    //getter
    std::string getPort(void) const {return this->_port;}
    int getListeningSocketFd(void) const {return this->_listeningSocketFd;}

    // void setSignal(int signal) {this->_signal = signal;} //true falseが入る
    // struct addrinfo *getAddress(){return this->_address;} //getter
};

std::ostream &operator<<(std::ostream &out, const Server &Server);
