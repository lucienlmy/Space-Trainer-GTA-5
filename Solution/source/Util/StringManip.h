#pragma once

#include <string>

// Integer/String manipulation
std::string IntToHexString(unsigned long i, bool zerox);

namespace boost
{
	// +ve numbers + ASCII (no UTF-8)
	void to_upper(std::string& str);
	std::string to_upper_copy(std::string str);

	void to_lower(std::string& str);
	std::string to_lower_copy(std::string str);
}

bool IsSafePath(const std::string& path);




