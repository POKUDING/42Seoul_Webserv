#include "errorHandle.hpp"

void errorHandle::printError()
{
	//std::cerr??
	std::cout << "Error: " << strerror(errno) << std::endl;
}

void errorHandle::printError(const std::string & msg)
{
	//std::cerr??
	std::cout << "Error: " << msg << std::endl;
}