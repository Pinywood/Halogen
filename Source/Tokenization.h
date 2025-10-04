#pragma once

#include <string>

bool TokenPresent(const std::string& string, const std::string& token, const std::string& before = "//");
bool TokenPresentAfter(const std::string& string, const std::string& token, const std::string& after);