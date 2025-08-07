#include "../include/Channel.hpp"
#include "../include/Client.hpp"
#include "../include/Server.hpp"


// void SignalHandler(int signum)
// {
//     Server server;
//     server.setSignal(true);
//     (void)signum;
// }

int main(int argc,char *argv[])
{
    Server Server;
    if (argc != 2)
    {
        std::cout << "Error: ./ircserv <port>" << std::endl;
        return (1);
    }
    try{
        // signal(SIGINT, SignalHandler); //ctl+c
        // signal(SIGQUIT, SignalHandler);//ctl+バックスラッシュ
        Server.ServerInit(argv);
        // std::cout << Server << std::endl;
        Server.ServerRun();
    }
    catch(const std::exception &e){
        std::cerr << e.what() << std::endl;
    }
    std::cout << "The Server Closed!" << std::endl;
}
