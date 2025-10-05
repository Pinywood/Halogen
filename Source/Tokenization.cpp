#include "Tokenization.h"

bool TokenPresent(const std::string& string, const std::string& token, const std::string& before)
{
	const auto& found = string.find(token);
	if (found != std::string::npos && found < string.find(before))
		return true;

	return false;
}

bool TokenPresentAfter(const std::string& string, const std::string& token, const std::string& after)
{
	if (string.find(token) == std::string::npos)
		return false;

	const size_t& AfterPos = string.find(after);
	if (AfterPos == std::string::npos)
		return true;

	if (string.find(token, AfterPos) != std::string::npos)
		return true;

	return false;
}

std::string GetToken(const std::string& string, const std::string& delim)
{
	std::string token;

	size_t found = string.find(delim);
	if (found == std::string::npos)
		found = string.length();

	char c;
	for (int i = 0; i < found; i++)
	{
		c = string.at(i);
		if (isalnum(c) || c == '_')
			token.push_back(c);
	}

	return token;
}

std::string GetToken(const std::string& string, const size_t& start, const std::string& delim)
{
	std::string token;

	size_t found = string.find(delim, start);
	if (found == std::string::npos)
		found = string.length();

	char c;
	for (int i = start; i < found; i++)
	{
		c = string.at(i);
		if (isalnum(c) || c == '_')
			token.push_back(c);
	}

	return token;
}
