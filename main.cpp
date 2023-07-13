#include <iostream>
#include "errorHandle.hpp"
#include "config.hpp"

int	main(int argc, char* argv[], char** env)
{
	if (argc > 2) {
		errorHandle::printError(ERR_INPUT_FILE);
		exit(EXIT_SUCCESS);
	}

	//Config() => parsing start
	Config config;

	argv[1] ? config.parse(argv[1]) : config.parse();
	
	(void)env;


		
	return 0;
}