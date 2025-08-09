#include "../include/echoServer.hpp"

// void SignalHandler(int signum)
// {
//     echoServer server;
//     server.setSignal(true);
//     (void)signum;
// }

int main(int argc, char *argv[])
{
    echoServer Server;
    if (argc != 2)
    {
        std::cout << "Error: ./echoServ <port>" << std::endl;
        return (1);
    }
    try
    {
        // signal(SIGINT, SignalHandler); //ctl+c
        // signal(SIGQUIT, SignalHandler);//ctl+バックスラッシュ
        Server.echoserverInit(argv);
        // std::cout << Server << std::endl;
        Server.echoserverRun();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    std::cout << "The Server Closed!" << std::endl;
}
