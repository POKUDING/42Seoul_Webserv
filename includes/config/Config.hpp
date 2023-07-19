#pragma once
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <map>
#include <cstring>
#include <stdexcept>
#include "Server.hpp"
#include "../util/SpiderMenUtil.hpp"
#include "../basic.hpp"

using namespace std;


class Config
{
	public:
		Config();

		//Config parsing related function to be added
		void					parse(const string& file = DEFAULT_FILE);
		// void					printMembers() const;

		const map< int, vector<Server> >&	getServer() const;

		// void					addServer(Server& Server);
		

	private:
		map< int, vector<Server> >	mServer;
};



#endif //CONFIG_HPP
