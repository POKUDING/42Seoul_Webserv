#ifndef HEAD_HPP
#define HEAD_HPP

#include <string>

using namespace std;

class Head
{
public:
	Head();
	~Head();

	string&	getHeadBuffer();
	int		addHead(string& inputBuff);
private:
	string	mHeadBuffer;
};


#endif