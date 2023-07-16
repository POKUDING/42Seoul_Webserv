#include <iostream>
#include "ErrorHandle.hpp"
#include "Config.hpp"

int	main(int argc, char* argv[], char** env)
{
	(void)env;
	
	if (argc > 2) {
		ErrorHandle::printError(ERR_INPUT_FILE);
		exit(EXIT_FAILURE);
	}

	//Config() => parsing start
	Config Config;

	try {

		argv[1] ? Config.parse(argv[1]) : Config.parse();

		//print all
		
	} catch (const exception& e) {
		cout << e.what() << endl;
		exit(EXIT_FAILURE);
	}
	cout <<"\n\nsucess\n" <<endl;
	Config.printMembers();
	


		
	return EXIT_SUCCESS;
}