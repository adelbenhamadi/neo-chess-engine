#pragma once
#include <sstream>
#include <vector>
#include <string>

namespace Helpers {
	inline std::vector<std::string> splitStringstream(std::stringstream& ss, char del) {
		std::vector<std::string> res;
		std::string s;
		while (std::getline(ss, s, del)) {
			res.push_back(s);
		}
		return res;
	}
	inline unsigned int charToi(char c) { return c - '0'; }

}