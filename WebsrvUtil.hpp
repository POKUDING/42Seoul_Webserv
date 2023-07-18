#ifndef WEBSRVUTIL_HPP
# define WEBSRVUTIL_HPP

# include <vector>
# include <string>
# include <cctype>

using namespace std;

class WebsrvUtil
{
	public:
		static vector<string>	splitString(string str, string del);
		static vector<string>	splitString(string str);
	private:
		WebsrvUtil();
		WebsrvUtil(WebsrvUtil& src);
		WebsrvUtil& operator = (WebsrvUtil& src);
		~WebsrvUtil();
};

#endif