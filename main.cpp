#include <iostream>
#include "errorHandle.hpp"

int	main(int argc, char* argv[], char** env)
{
	if (argc > 2)
		errorHandle::printError(ERR_INPUT_FILE);

	return 0;
}