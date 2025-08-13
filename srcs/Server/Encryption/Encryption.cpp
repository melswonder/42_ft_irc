#include "../../../include/IRC.hpp"

std::string Server::xorEncryptDecrypt(const std::string& data)
{
	std::string key = KEY;
	std::string result = data;
	size_t key_len = key.length();
	for(size_t i = 0; i < data.length(); i++)
	{
		result[i] = data[i] ^ key[i % key_len];
	}
	return result;
}
