#include "StringManip.h"

#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>

// Integer/String manipulation
std::string IntToHexString(unsigned long i, bool zerox)
{
	std::stringstream stream;
	if (zerox)
		stream << "0x" << std::setfill('0') << std::setw(sizeof(i) * 2);
	stream << std::hex << i;
	return stream.str();
}

namespace boost
{
	// +ve numbers + ASCII (no UTF-8)
	void to_upper(std::string& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	}
	std::string to_upper_copy(std::string str)
	{
		std::transform(str.begin(), str.end(), str.begin(), ::toupper);
		return str;
	}

	void to_lower(std::string& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	}
	std::string to_lower_copy(std::string str)
	{
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		return str;
	}
}

bool IsSafePath(const std::string& path)
{
	if (path.empty() || path.length() > 128) return false;

	for (unsigned char c : path)
	{
		if (!isalnum(c) && c != '_' && c != '-' && c != ' ')
		{
			return false;
		}
	}

	std::string upper = path;
	std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

	static const char* const reservedNames[] = {
		"CON", "PRN", "AUX", "NUL",
		"COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
		"LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"
	};

	for (const std::string& reserved : reservedNames)
	{
		if (upper == reserved) return false;
	}

	if (path.front() == ' ' || path.back() == ' ' || path.back() == '.') return false;

	return true;
}



