#pragma once
#ifndef ErrorHandle_HPP
#define ErrorHandle_HPP

#include <iostream>
#include <cerrno>
#include <cstring>

#define ERR_INPUT_FILE "Put 1 file or none"
#define ERR_CONFIG "Config file format wrong"

class ErrorHandle
{

public:
	// static void printError();
	static void printError(const std::string & msg = strerror(errno));
	
};

#endif //end of ErrorHandle_HPP
