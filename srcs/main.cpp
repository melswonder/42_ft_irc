#include "../include/Channel.hpp"
#include "../include/Client.hpp"
#include "../include/Server.hpp"

// void SignalHandler(int signum)
// {
//     Server server;
//     server.setSignal(true);
//     (void)signum;
// }

int isValidArgument(int argc, char *argv[])
{
    if (argc != 3)
        return (1);
    std::string port = argv[1];
    std::string password = argv[2];
    if (port.empty() || password.empty())
        return (1);
    return (0);
}

int main(int argc, char *argv[])
{
    Server Server;
    if (isValidArgument(argc, argv))
    {
        std::cout << "Error: ./ircserv <port> <password>" << std::endl;
        return (1);
    }
    try
    {
        // signal(SIGINT, SignalHandler); //ctl+c
        // signal(SIGQUIT, SignalHandler);//ctl+バックスラッシュ
        Server.ServerInit(argv);
        // std::cout << Server << std::endl;
        Server.ServerRun();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    // std::cout << "The Server Closed!" << std::endl;
}
