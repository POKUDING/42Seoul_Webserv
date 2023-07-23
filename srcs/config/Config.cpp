#include "../../includes/config/Config.hpp"

Config::Config() { }

void					Config::parse(const string& file)
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
		vector<string> temp = SpiderMenUtil::splitString(line);
		
		if (temp.size() == 0 || temp[0][0] == '#')
			continue;
		if (temp.size() == 2 && temp[0] == "server" && temp[1] == "{") {
			Server server;

			server.parse(f_dataRead);
			map<int, vector<Server> >::iterator it = mServer.find(server.getListen());

			if (it == mServer.end())
			{
				vector<Server> servec_tmp;
				servec_tmp.push_back(server);
				mServer.insert(pair<int, vector<Server> >(server.getListen(), servec_tmp));				
			}
			else
				it->second.push_back(server);

			// if (it != mServer.end())
			// 	it->second.push_back(server);
			// else
			// {
			// 	vector<Server> servec_tmp;
			// 	servec_tmp.push_back(server);
			// 	mServer.insert(pair<int, vector<Server> >(server.getListen(), servec_tmp));
			// }
		} else {
			cout << "Wrong line: " << line << endl;
			throw runtime_error("Error: config wrong arguments.");
			// f_dataRead.close();
			exit(EXIT_FAILURE);
		}
	}
	// close
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

// void					Config::addServer(Server& Server) { this->mServer.push_back(Server); }
