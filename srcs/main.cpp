#include "../includes/IRC.hpp"

void SignalHandler(int signum)
{
    (void)signum;
	throw std::runtime_error("Quit");
}

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
	Server server;
	if (isValidArgument(argc, argv))
	{
		std::cout << "Error: ./ircserv <port> <password>" << std::endl;
		return (1);
	}
	try
	{
		signal(SIGINT, SignalHandler);
		signal(SIGQUIT, SignalHandler);
		server.serverInit(argv);
		server.serverRun();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	// std::cout << "The Server Closed" << std::endl;
}
