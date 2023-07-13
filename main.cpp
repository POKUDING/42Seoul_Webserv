#include <iostream>
#include "errorHandle.hpp"

int	main(int argc, char* argv[], char** env)
{
	if (argc > 2) {
		errorHandle::printError(ERR_INPUT_FILE);
		exit(EXIT_SUCCESS);
	}
	
	//open conf file
	if (argc == 0) {
		// ./default.conf 사용
	} else {
		//변수파일 사용
	}

	//if open OK => parsing start

	
		
	return 0;
}