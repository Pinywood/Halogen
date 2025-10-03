#include "Tokenization.h"

bool TokenPresent(const std::string& string, const std::string& token)
{
	const auto& found = string.find(token);
	if (found != std::string::npos && found < string.find("//"))
		return true;

	return false;
}
