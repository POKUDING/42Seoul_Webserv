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

		SpiderMen websrv(config);
		websrv.run();
	} catch (const exception& e) {
		cout << "this is main catch: " << e.what() << endl;
		exit(EXIT_FAILURE);
	}

	//webserv Class 생성
		//socket ~ bind ~ listen
			//what is BACKLOG
		//kqueue & kevent
		//while
			//kqueue 등록 (new client)
			//del client (연결 끊기)
			//request 받음
				//Request Class (ac) 생성
					// request 상속 -> Get Class 생성
					// request 상속 -> Post Class 생성
					// request 상속 -> Delete Class 생성
				//Client Class 생성

				//parsing
				//cgi 처리
				//
	return EXIT_SUCCESS;
}