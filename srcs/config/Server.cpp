#include "../../includes/config/Server.hpp"

// constructor

Server::Server(): mRoot("/"), mListen(80), mClientMaxBodySize(MAX_BODY_SIZE) { }

// member functions

// public

void	Server::parse(ifstream& f_dataRead)
{
	string line;
    while (getline(f_dataRead, line)) {

		if (line.length() == 0)
			continue;
			
		//split here
		vector<string> splitedLine = SpiderMenUtil::splitString(line);
		if (splitedLine.size() == 0 || splitedLine[0][0] == '#')
			continue;
		if (splitedLine[0][0] == '}' && splitedLine.size() == 1) {
			return ;//end parsing
		} else if (splitedLine.size() < 2) {
			throw runtime_error("Error: Invalid server: key - value");
		} else if (splitedLine[0] == "location" && splitedLine[2] == "{" && splitedLine.size() == 3) {
			Location location(splitedLine[1]);
			location.parse(f_dataRead);
			this->addLocation(location);
		} else {
			if (!(line.length() - 1 == line.find(';')))
				throw runtime_error("Error: Invalid location: ';'");
				
			line = line.substr(0, line.length() - 1);
			splitedLine = SpiderMenUtil::splitString(line);
			setValue(splitedLine);
		}
	}
}

void	Server::setValue(const vector<string>& splitedLine)
{
	if (splitedLine[0] == "error_page" && splitedLine.size() > 2) {
		for (size_t i = 1; i < splitedLine.size() - 1; i++) {
			this->addErrorPage(splitedLine[i], splitedLine[splitedLine.size() - 1]);
		}
		return ;
	} else if (splitedLine[0] == "server_name") {
		for (size_t i = 1; i < splitedLine.size(); i++) {
			this->addServerName(splitedLine[i]);
		}
		return ;
	} else if (splitedLine.size() != 2) {
		throw runtime_error("Error: Invalid server1");
	}
	if (splitedLine[0] == "root") {
		this->setRoot(splitedLine[1]);
	} else if (splitedLine[0] == "listen") {
		this->setListen(splitedLine[1]);
	} else if (splitedLine[0] == "client_max_body_size") {
		this->setClientMaxBodySize(splitedLine[1]);
	} else {
		throw runtime_error("Error: Invalid server: wrong key");
	}
}

// getters and setters

const vector<Location>&		Server::getLocation() const { return this->mLocation; }
const vector<string>&		Server::getServerName() const { return this->mServerName; }
const map<string, string>&	Server::getErrorPage() const { return this->mErrorPage; }
const string&				Server::getRoot() const { return this->mRoot; }
int							Server::getListen() const { return this->mListen; }
int							Server::getClientMaxBodySize() const { return this->mClientMaxBodySize; }

void						Server::addLocation(const Location& mLocation) { this->mLocation.push_back(mLocation); }
void						Server::addServerName(const string& mServerName) { this->mServerName.push_back(mServerName); }
void						Server::addErrorPage(const string& code, const string& page) { this->mErrorPage.insert(pair<string, string>(code, page)); }
void						Server::setRoot(const string& mRoot) { this->mRoot = mRoot; }
void						Server::setListen(const string& mListen)
{
	int	portNumber;

	//int check
	if (mListen.size() > 5)
		throw runtime_error("Error: too big listen port");
	for(size_t i = 0; i < mListen.size(); ++i)
	{
	// 	cout << mListen << endl;
		if (isdigit(mListen[i]) == false)
			throw runtime_error("Error: invalid listen port");
	}
	portNumber = atoi(mListen.c_str());
	if (portNumber < 1 || portNumber > MAX_PORT_SIZE)
		throw runtime_error("Error: invlid listen port");
	// if (!(1 <= portNumber && portNumber <= MAX_PORT_SIZE))
	// 	throw runtime_error("Error: invlid listen port");

	this->mListen = portNumber;
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

// print

void						Server::printMembers() const
{
	cout << "	ErrorPage: \n";
	printMap(this->getErrorPage());
	cout << "	ServerName: \n";
	for (size_t j = 0; j < this->getServerName().size(); ++j)
		cout << "\t\t" << j << ": " << this->getServerName()[j] << "\n";
	cout << "	Root: " << this->getRoot() << "\n";
	cout << "	Listen: " << this->getListen() << "\n";
	cout << "	ClientMaxBodySize: " << this->getClientMaxBodySize() << "\n";
	for (size_t j = 0, end = this->getLocation().size(); j < end; ++j)
	{
		cout << "	Location [" << this->getLocation()[j].getKey() << "] --------------\n";
		this->getLocation()[j].printMembers();
	}
}
