#pragma once

#include <string>

bool TokenPresent(const std::string& string, const std::string& token, const std::string& before = "//");
bool TokenPresentAfter(const std::string& string, const std::string& token, const std::string& after);
std::string GetToken(const std::string& string, const std::string& delim = "\n");
std::string GetToken(const std::string& string, const size_t& start, const std::string& delim = "\n");