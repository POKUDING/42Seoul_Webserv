#include "Config.hpp"

//Location ===============================
Location::Location(const string& mKey): mKey(mKey) { memset(this, 0, sizeof(Location)); }

void	Location::setValue(const vector<string>& temp)
{
	if (temp[0] == "limit_except") {
		for (size_t i = 1; i < temp.size(); i++) {
			this->addLimitExcept(temp[i]);
		}
		return ;
	} else if (temp.size() != 2) {
		throw runtime_error("Error: Invalid location: too many values");
	}

	if (temp[0] == "root") {
		this->setRoot(temp[1]);
	} else if (temp[0] == "fastcgi_pass") {
		this->setFastcgiPass(temp[1]);
	} else if (temp[0] == "index") {
		this->setIndex(temp[1]);
	} else if (temp[0] == "autoindex") {
		this->setAutoIndex(temp[1]);
	} else if (temp[0] == ".sh") {
		this->setSh(temp[1]);
	} else if (temp[0] == ".py") {
		this->setPy(temp[1]);
	} else if (temp[0] == ".php") {
		this->setPhp(temp[1]);
	} else if (temp[0] == "return") {
		this->setReturn(temp[1]);
	} else {
		throw runtime_error("Error: Invalid location: no such key");
	}
}

void					Location::parse(ifstream& f_dataRead)
{
	string line;
    while (getline(f_dataRead, line)){

		if (line.length() == 0)
			continue;

		cout << "LOCATION: " << line << endl;

		// if (!(line.length() - 1 == line.find(';')) && line.find('}') != string::npos)
		// 	throw runtime_error("Error: Invalid location: ';'");
		// line = line.substr(0, line.length() - 1);
		//split here
		vector<string> temp = WebsrvUtil::splitString(line);
		if (temp.size() == 0 || temp[0][0] == '#')
			continue;
		if (temp[0][0] == '}' && temp.size() == 1) {
			return ;//end parsing
		} else if (temp.size() < 2) {
			throw runtime_error("Error: Invalid location: too less value");
		} else {
			if (!(line.length() - 1 == line.find(';')))
				throw runtime_error("Error: Invalid location: ';'");
			
			line = line.substr(0, line.length() - 1);
			temp = WebsrvUtil::splitString(line);
			setValue(temp);
		}
	}
}

const string&			Location::getKey() const { return this->mKey; }
const string&			Location::getRoot() const { return this->mRoot; }
const vector<string>&	Location::getLimitExcept() const { return this->mLimitExcept; }
const string&			Location::getFastcgiPass() const { return this->mFastcgiPass; }
const string&			Location::getIndex() const { return this->mIndex; }
bool					Location::getAutoIndex() const { return this->mAutoIndex; }
const string&			Location::getSh() const { return this->mSh; }
const string&			Location::getPy() const { return this->mPy; }
const string&			Location::getPhp() const { return this->mPhp; }
const string&			Location::getReturn() const { return this->mReturn; }

void					Location::setRoot(const string& mRoot) { this->mRoot = mRoot; }
void					Location::addLimitExcept(const string& mLimitExcept)
{
	if (mLimitExcept == "GET" || mLimitExcept == "POST" || mLimitExcept == "DELETE")
		this->mLimitExcept.push_back(mLimitExcept);
	else
		throw runtime_error("Error: Invalid location: Limit Except");
}
void					Location::setFastcgiPass(const string& mFastcgiPass) { this->mFastcgiPass = mFastcgiPass; }
void					Location::setIndex(const string& mIndex) { this->mIndex = mIndex; }
void					Location::setAutoIndex(const string& mAutoIndex)
{
	if (mAutoIndex == "on")
		this->mAutoIndex = true;
	else if (mAutoIndex == "off")
		this->mAutoIndex = false;
	else
		throw runtime_error("Error: Invalid location: Auto Index");
}
void					Location::setSh(const string& mSh) { this->mSh = mSh; }
void					Location::setPy(const string& mPy) { this->mPy = mPy; }
void					Location::setPhp(const string& mPhp) { this->mPhp = mPhp; }
void					Location::setReturn(const string& mReturn) { this->mReturn = mReturn; }

//Server ===============================
Server::Server() { memset(this, 0, sizeof(Server)); }

void	Server::setValue(const vector<string>& temp)
{
	if (temp[0] == "error_page" && temp.size() > 2) {
		for (size_t i = 1; i < temp.size() - 1; i++) {
			
			cout << &(this->mErrorPage) << endl;
			cout << temp[i] << endl;
			cout << temp[temp.size() - 1] << endl;
			// this->mErrorPage[temp[i]] = temp[temp.size() - 1];
			cout << "111111" << endl;

			this->mErrorPage.insert(pair<string, string>("asd", "asd"));
			// make_pair()
			pair<string, string> p = pair<string, string>("a", "b");
			cout << "111111" << endl;
			// cout << p << endl;
			cout << "22222" << endl;
			this->mErrorPage.insert(p);
			cout << "2233333" << endl;

			// this->addErrorPage(temp[i], temp[temp.size() - 1]);
			const vector<string>::const_iterator it = temp.begin() + i - 1;
			const vector<string>::const_iterator it2 = temp.end() - 1;

			string a = *it;
			string b = *it2;
			this->mErrorPage.insert(pair<string, string>(a, b));
			return ;
		}
		return ;
	} else if (temp[0] == "server_name") {
		for (size_t i = 1; i < temp.size() - 1; i++) {
			this->addServerName(temp[i]);
		}
		return ;
	} else if (temp.size() != 2) {
		throw runtime_error("Error: Invalid server1");
	}
	if (temp[0] == "root") {
		this->setRoot(temp[1]);
	} else if (temp[0] == "listen") {
		this->setListen(temp[1]);
	} else if (temp[0] == "client_max_body_size") {
		this->setClientMaxBodySize(temp[1]);
	} else {
		throw runtime_error("Error: Invalid server: wrong key");
	}
}


void					Server::parse(ifstream& f_dataRead)
{
	string line;
    while (getline(f_dataRead, line)) {

		if (line.length() == 0)
			continue;
			
		//split here
		vector<string> temp = WebsrvUtil::splitString(line);
		
		cout << "SERVER: " << line << endl;

		if (temp.size() == 0 || temp[0][0] == '#')
			continue;
		if (temp[0][0] == '}' && temp.size() == 1) {
			cout << "end of server }" << endl;
			return ;//end parsing
		} else if (temp.size() < 2) {
			throw runtime_error("Error: Invalid server: key - value");
		} else if (temp[0] == "location" && temp[2] == "{" && temp.size() == 3) {
			Location location(temp[1]);
			location.parse(f_dataRead);
			this->addLocation(location);
		} else {
			line = line.substr(0, line.length() - 1);
			temp = WebsrvUtil::splitString(line);
			setValue(temp);
		}
	}
}

const vector<Location>&		Server::getLocation() const { return this->mLocation; }
const map<string, string>&	Server::getErrorPage() const { return this->mErrorPage; }
const vector<string>&		Server::getServerName() const { return this->mServerName; }
const string&				Server::getRoot() const { return this->mRoot; }
int							Server::getListen() const { return this->mListen; }
int							Server::getClientMaxBodySize() const { return this->mClientMaxBodySize; }

void						Server::addLocation(const Location& mLocation) { this->mLocation.push_back(mLocation); }
void						Server::addErrorPage(const string& code, const string& page) { this->mErrorPage.insert(pair<string, string>(code, page)); }
void						Server::addServerName(const string& mServerName) { this->mServerName.push_back(mServerName); }
void						Server::setRoot(const string& mRoot) { this->mRoot = mRoot; }

void						Server::setListen(const string& mListen)
{
	int	tmp;
	///need to modify str -> int
	//1~65535 port available

	//int check
	cout<< mListen << endl;
	if (mListen.size() > 5)
		throw runtime_error("Error: invalid listen port");
	for(size_t i = 0; i < mListen.size(); i++)
	{
		if (!isdigit(mListen[i]))
			throw runtime_error("Error: invalid listen port");
	}
	tmp = atoi(mListen.c_str());
	if (tmp < 1 || tmp > MAX_PORT_SIZE)
		throw runtime_error("Error: invlid listen port");
	this->mListen = tmp;


}
void						Server::setClientMaxBodySize(const string& mClientMaxBodySize)
{
	int	tmp;

	///need to modify str -> int
	if (mClientMaxBodySize.size() > 6)
		throw runtime_error("Error: invalid maxbodysize: too big");
	for(size_t i = 0; i < mClientMaxBodySize.size(); i++)
	{
		if (!isdigit(mClientMaxBodySize[i]))
			throw runtime_error("Error: invalid maxbodysize: not digit");
	}
	tmp = atoi(mClientMaxBodySize.c_str());
	if (tmp < 1 || tmp > MAX_BODY_SIZE)
		throw runtime_error("Error: invalid maxbodysize: not in range");
	this->mClientMaxBodySize = tmp;

}


//Config ===============================
Config::Config() { }

void					Config::parse(const string& file)
{
	//file open
	ifstream f_dataRead(file, ios_base::in);
    if (!f_dataRead) {
		ErrorHandle::printError();
		exit(EXIT_FAILURE);
	}

	// parsing
	string line;
    while (getline(f_dataRead, line)){

		if (line.length() == 0)
			continue;
		
		cout << "CONFIG: " << line << endl;
		
		//split here
		vector<string> temp = WebsrvUtil::splitString(line);
		
		if (temp[0][0] == '#') {
			continue;
		} else if (temp[0] == "server" && temp[1] == "{" && temp.size() == 2) {
			Server server;
			server.parse(f_dataRead);
			cout << &this->mServer << endl;

			
			this->mServer.push_back(server);
		} else {
			cout << line << endl;
			ErrorHandle::printError(ERR_CONFIG);
			// f_dataRead.close();
			exit(EXIT_FAILURE);
		}
	}
	// close
	f_dataRead.close();
}

void	Config::printMembers()
{
	for (size_t i = 0; i < mServer.size(); ++i)
	{
		cout << "Server " << i << ": \n";
		cout << "ErrorPage: \n";
		printMap(mServer[i].getErrorPage());
		cout << "ServerName: \n";
		for (size_t j = 0; j < mServer[i].getServerName().size(); ++j)
			cout << j << ": " << mServer[i].getServerName()[j] << "\n";
		cout << "Root: " << mServer[i].getRoot() << "\n";
		cout << "Listen: " << mServer[i].getListen() << "\n";
		cout << "ClientMaxBodySize: " << mServer[i].getClientMaxBodySize() << "\n";
		for (size_t j = 0; j < mServer[i].getLocation().size(); ++j)
		{
			cout << "\tLocation" << mServer[i].getLocation()[j].getKey() << ": \n";
			cout << "\tLimitExcept: \n";
			for (size_t k = 0; k < mServer[i].getLocation()[j].getLimitExcept().size(); ++k)
				cout << "\t" << k << ": " << mServer[i].getLocation()[j].getLimitExcept()[j] << "\n";
			cout << "\tRoot: " << mServer[i].getLocation()[j].getRoot() << "\n";
			cout << "\tFastcgiPass: " << mServer[i].getLocation()[j].getFastcgiPass() << "\n";
			cout << "\tIndex: " << mServer[i].getLocation()[j].getIndex() << "\n";
			cout << "\tAutoIndex: " << mServer[i].getLocation()[j].getAutoIndex() << "\n";
			cout << "\tSh: " << mServer[i].getLocation()[j].getSh() << "\n";
			cout << "\tPy: " << mServer[i].getLocation()[j].getPy() << "\n";
			cout << "\tPhp: " << mServer[i].getLocation()[j].getPhp() << "\n";
			cout << "\tSh: " << mServer[i].getLocation()[j].getReturn() << "\n";
		} 
	}
}


const vector<Server>&	Config::getServer() const { return this->mServer; }

void					Config::addServer(Server Server) { this->mServer.push_back(Server); }
