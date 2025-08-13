#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h> //-> for socket()
#include <sys/types.h>	//-> for socket()
#include <netinet/in.h> //-> for sockaddr_in
#include <fcntl.h>		//-> for fcntl()
#include <unistd.h>		//-> for close()
#include <arpa/inet.h>	//-> for inet_ntoa()
#include <poll.h>		//-> for poll()
#include <csignal>		//-> for signal()
#include <signal.h>		// signal関数を使うために必要
#include <netdb.h>		// getaddrinfoを使う
#include <cstring>		// memset()
#include <cerrno>		// errno
#include <stdlib.h>		// atoi
#include <algorithm>
#include <map>

#define RED "\033[1;31m" //-> for red color
#define WHI "\033[0;37m" //-> for white color
#define GRE "\033[1;32m" //-> for green color
#define YEL "\033[1;33m" //-> for yellow color

class Client;
class Channel;
class Server;

#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "Helper.hpp"
#include "Error.hpp"
