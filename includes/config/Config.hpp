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
		Config(string fileName);

		void								parse(const string& file = DEFAULT_FILE);
		const map< int, vector<Server> >&	getServer() const;
		// void					printMembers() const;

	private:
		map< int, vector<Server> >	mServer;
};

#endif // CONFIG_HPP