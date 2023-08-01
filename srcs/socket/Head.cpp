#include "../../includes/socket/Head.hpp"

// constructor and destructor

Head::Head() {}
Head::~Head() {}

// member functions

// pubilc

void	Head::clear() { mHeadBuffer.clear(); }

int	Head::addHead(string& inputBuff)
{	
	// cout << "\n==== input buffer size: " << inputBuff.size() << endl;
	size_t	pos = inputBuff.find("\r\n\r\n");
	if (pos != string::npos)
	{
		mHeadBuffer.append(inputBuff.c_str(), pos);
		if (pos + 4 != inputBuff.size())
			inputBuff = inputBuff.substr(pos + 4);
		else
			inputBuff.clear();
		cout << "after remove header, in input buffer: $" << inputBuff << "$" << endl;
		return 1;
	}
	return 0;
}

// getters and setters

string& Head::getHeadBuffer() { return mHeadBuffer; }