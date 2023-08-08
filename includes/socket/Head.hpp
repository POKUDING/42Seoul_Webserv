#ifndef HEAD_HPP
#define HEAD_HPP

#include <string>
#include <iostream>
#include "../util/InputBuffer.hpp"

using namespace std;

class Head
{
	public:
		Head();
		~Head();
		void	clear();
		int		addHead(InputBuffer& inputBuff);
		
		string&	getHeadBuffer();
	private:
		string	mHeadBuffer;
};

#endif // HEAD_HP