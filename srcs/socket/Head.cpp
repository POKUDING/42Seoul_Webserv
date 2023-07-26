#include "../../includes/socket/Head.hpp"

Head::Head() {}

Head::~Head() {}

string& Head::getHeadBuffer() { return mHeadBuffer; }

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
