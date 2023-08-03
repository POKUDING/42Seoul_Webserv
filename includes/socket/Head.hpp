#ifndef HEAD_HPP
#define HEAD_HPP

#include <string>
#include <iostream>

using namespace std;

class Head
{
	public:
		Head();
		~Head();

		void	clear();
		int		addHead(string& inputBuff);
		
		string&	getHeadBuffer();
	private:
		string	mHeadBuffer;
};

#endif // HEAD_HPP