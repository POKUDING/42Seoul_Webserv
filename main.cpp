#include <iostream>
#include "ErrorHandle.hpp"
#include "Config.hpp"
#include "Websrv.hpp"

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

	//Config() => parsing start
	Config config;

	try {
		//config parse => Config에 저장
		argv[1] ? config.parse(argv[1]) : config.parse();
		// Config.printMembers();

		Websrv websrv(config);
		
	} catch (const exception& e) {
		cout << e.what() << endl;
		exit(EXIT_FAILURE);
	}
	
	//webserv Class 생성
		//socket ~ bind ~ listen
			//what is BACKLOG
		//kqueue & kevent
		//while
			//kqueue 등록 (new client)
			//del client (연결 끊기)
			//get request
				//parsing
				//cgi 처리
				//
	


		
	return EXIT_SUCCESS;
}