#include <iostream>
#include "../includes/config/Config.hpp"
#include "../includes/SpiderMen.hpp"

void ft_leak()
{
	system("leaks webserv");
}

int	main(int argc, char* argv[], char** env)
{
	(void)env;
	
	// atexit(ft_leak);
	if (argc > 2) {
		throw runtime_error("Error: too many arguments.");
		exit(EXIT_FAILURE);
	}

	Config config;
	try {
		if (argc == 2)
			Config config(argv[1]);
		else
			Config config;
		// Config.printMembers();

		SpiderMen spiderMen(config);
		spiderMen.run();
	} catch (const exception& e) {
		cout << "this is main catch: " << e.what() << endl;
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}