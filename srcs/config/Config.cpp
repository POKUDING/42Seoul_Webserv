#include "../../includes/config/Config.hpp"

// constructors

Config::Config() { }
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
    while (getline(f_dataRead, line)) {

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

			if (it == mServer.end()) {
				vector<Server> tempServer;
				tempServer.push_back(server);
				mServer.insert(pair<int, vector<Server> >(server.getListen(), tempServer));				
			} else {

				vector<Server>::iterator sit = it->second.begin();
				vector<Server>::iterator send = it->second.end();
				int hostsize = server.getServerName().size();
				for (; sit != send; ++sit) {
					for (int i = 0; i < hostsize; ++i) {
						if (find(sit->getServerName().begin(), sit->getServerName().end(), server.getServerName()[i]) != sit->getServerName().end())
							throw runtime_error("Error: config wrong arguments - duplicated server_name.");
					}
				}
				it->second.push_back(server);
			}
		} else {
			throw runtime_error("Error: config wrong arguments.");
		}
	}
	f_dataRead.close();

	// '/' location 있는지 확인, dir있는지 확인
	for (map<int, vector<Server> >::iterator it = mServer.begin(); it != mServer.end(); ++it) {
		for (size_t i = 0, end = (it->second).size(); i < end; ++i) {
			size_t j = 0, jend = (it->second[i]).getLocation().size(), root = 0;
			for ( ; j < jend; ++j) {
				string key = (it->second[i]).getLocation()[j].getKey();
				if (key.size() && key.at(key.size() - 1) == '/') {
					//location root확인
					//없으면 server root + key 값으로 확인
					string location = (it->second[i]).getLocation()[j].getRoot();
					if (location.size() == 0)
						location = (it->second[i]).getRoot() + key;
					if (access(location.c_str(), F_OK) < 0)
						runtime_error("Error: location block doesn't exist.");
				}
				if (key == "/")
					root = 1;
			}
			if (!root)
				throw runtime_error("Error: no '/' location in config.");
		}
	}
}

const map< int, vector<Server> >&	Config::getServer() const { return this->mServer; }
