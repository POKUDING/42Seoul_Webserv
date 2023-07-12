#include <iostream>
#include <cerrno>
#include <cstring>

#define ERR_INPUT_FILE "Put 1 file or none"

class errorHandle
{

public:
	static void printError();
	static void printError(const std::string & msg);
	
};