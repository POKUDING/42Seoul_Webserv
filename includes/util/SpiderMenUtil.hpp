#pragma once
#ifndef SPIDERMENUTIL_HPP
# define SPIDERMENUTIL_HPP

# include <vector>
# include <string>
# include <cctype>
# include <iostream>
# include <sstream>

using namespace std;

class SpiderMenUtil
{
	public:
		static vector<string>	splitString(string str, string del);
		static vector<string>	splitString(string str);
		static string			itostr(int num);
		static string 			replaceCRLF(const std::string& input);
	private:
		// SpiderMenUtil();
		// SpiderMenUtil(SpiderMenUtil& src);
		// SpiderMenUtil& operator = (SpiderMenUtil& src);
		// ~SpiderMenUtil();
};

#endif // SPIDERMENUTIL_HPP