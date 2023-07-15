#include "ErrorHandle.hpp"

// void ErrorHandle::printError()
// {
// 	//std::cerr??
// 	std::cout << "Error: " << strerror(errno) << std::endl;
// }

void ErrorHandle::printError(const std::string & msg)
{
	//std::cerr??
	std::cout << "Error: " << msg << std::endl;
}
