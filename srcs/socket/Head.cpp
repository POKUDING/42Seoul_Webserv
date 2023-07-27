#include "../../includes/socket/Head.hpp"

// constructor and destructor

Head::Head() {}
Head::~Head() {}

// member functions

// pubilc

void	Head::clear() { mHeadBuffer.clear(); }

int	Head::addHead(string& inputBuff)
{	
	size_t	pos = inputBuff.find("\r\n\r\n");
	if (pos != string::npos)
	{
		if (pos + 4 != inputBuff.size())
			inputBuff = inputBuff.substr(pos + 4);
		return 1;
	} else
		mHeadBuffer.append(inputBuff.c_str(), inputBuff.size());
	return 0;
}

// getters and setters

string& Head::getHeadBuffer() { return mHeadBuffer; }