#include <iostream>
#include "../includes/config/Config.hpp"
#include "../includes/SpiderMen.hpp"

// void ft_leak()
// {
// 	system("leaks webserv");
// }

void sigHandler(int sig) { (void) sig; exit(EXIT_SUCCESS); }

int	main(int argc, char* argv[])
{
	// atexit(ft_leak);
	

	try {

		if (argc > 2)
			throw runtime_error("Error: too many arguments.");
			
		Config config;
		if (argc == 2)
			config.parse(argv[1]);
		else
			config.parse();
		// Config.printMembers();
		signal(SIGINT, sigHandler);
		signal(SIGQUIT, sigHandler);
		while (42) {
			try {
				SpiderMen spiderMen(config);
				cout << "\n============================\n\n\n   server start ! \n\n\n============================" << endl;
				spiderMen.run();
			} catch (const exception& e) {
				cout << "server down => " << e.what() << endl;
				cout << "server restart!" << endl;
			}
		}
	} catch (const exception& e) {
		cout << "server cannot be started: " << e.what() << endl;
		exit(EXIT_FAILURE);
	}
	
	return EXIT_SUCCESS;
}
