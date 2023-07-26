#include "../../includes/config/Config.hpp"

// constructors

Config::Config() { parse(); }
Config::Config(string fileName) { parse(fileName); }

// member functions

// public

void	Config::parse(const string& file)
{
	//file open
	ifstream f_dataRead(file.c_str(), ios_base::in);
    if (!f_dataRead)
		throw	runtime_error("Error: conf file open failed.");

	// parsing
	string line;
    while (getline(f_dataRead, line) != 0) {

		if (line.length() == 0)
			continue;
		
		//split here
		vector<string> splitedLine = SpiderMenUtil::splitString(line);
		
		if (splitedLine.size() == 0 || splitedLine[0][0] == '#')
			continue;
		if (splitedLine.size() == 2 && splitedLine[0] == "server" && splitedLine[1] == "{") {
			Server server;

			server.parse(f_dataRead);
			map<int, vector<Server> >::iterator it = mServer.find(server.getListen());

			if (it == mServer.end())
			{
				vector<Server> tempServer;
				tempServer.push_back(server);
				mServer.insert(pair<int, vector<Server> >(server.getListen(), tempServer));				
			}
			else
				it->second.push_back(server);
		} else {
			// cout << "Wrong line: " << line << endl;
			throw runtime_error("Error: config wrong arguments.");
			exit(EXIT_FAILURE);
		}
	}
	f_dataRead.close();
}

// void	Config::printMembers() const
// {
// 	cout <<"\n\n========= printMembers() =========\n" <<endl;

// 	map< int, vector<Server> >::iterator it = mServer.begin();

// 	for (; it != mServer.end(); ++it)
// 	{
// 		cout << "Port [" << it->first << "] --------------\n";

// 		mServer[i].printMembers();
// 	}
// }

const map< int, vector<Server> >&	Config::getServer() const { return this->mServer; }
