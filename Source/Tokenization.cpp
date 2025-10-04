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
