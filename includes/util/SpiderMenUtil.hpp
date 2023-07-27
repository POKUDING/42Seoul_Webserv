#pragma once
#ifndef SPIDERMENUTIL_HPP
# define SPIDERMENUTIL_HPP

# include <vector>
# include <string>
# include <cctype>
# include <iostream>

using namespace std;

class SpiderMenUtil
{
	public:
		static vector<string>	splitString(string str, string del);
		static vector<string>	splitString(string str);

	private:
		// SpiderMenUtil();
		// SpiderMenUtil(SpiderMenUtil& src);
		// SpiderMenUtil& operator = (SpiderMenUtil& src);
		// ~SpiderMenUtil();
};

#endif // SPIDERMENUTIL_HPP