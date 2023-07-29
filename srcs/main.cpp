#include <iostream>
#include "../includes/config/Config.hpp"
#include "../includes/SpiderMen.hpp"

char **env;

void ft_leak()
{
	system("leaks webserv");
}

int	main(int argc, char* argv[], char** envp)
{
	env = envp;
	
	// atexit(ft_leak);
	if (argc > 2) {
		throw runtime_error("Error: too many arguments.");
		exit(EXIT_FAILURE);
	}

	try {
		Config config;
		if (argc == 2)
			config.parse(argv[1]);
		else
			config.parse();
		// Config.printMembers();

		SpiderMen spiderMen(config);
		cout << "\n============================\n\n\n   server start ! \n\n\n============================" << endl;
		spiderMen.run();
	} catch (const exception& e) {
		cout << "this is main catch: " << e.what() << endl;
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}