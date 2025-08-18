#include "../../include/IRC.hpp"

int Server::setNonblocking(int fd)
{
	int	flags;

	flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
	{
		std::cerr << "fcntl () error:" << std::strerror(errno) << std::endl;
		return (-1);
	}
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	return (0);
}

std::vector<std::string> split(const std::string &str, char delimiter)
{
	std::vector<std::string> tokens;

	if (str.size() <= 1)
	{
		tokens.push_back(str);
		return (tokens);
	}
	std::string::size_type start = 0;
	std::string::size_type end = str.find(delimiter);
	while (end != std::string::npos)
	{
		tokens.push_back(str.substr(start, end - start));
		start = end + 1;
		end = str.find(delimiter, start);
	}
	tokens.push_back(str.substr(start));
	return (tokens);
}

std::ostream &operator<<(std::ostream &out, const std::vector<std::string> &data)
{
	for (size_t i = 0; i < data.size(); ++i)
	{
		if (!data[i].empty())
			out << "[" << i << "]" << data[i] << std::endl;
	}
	return (out);
}
// /connect localhost 8080 a nicknamedayo userdayo
