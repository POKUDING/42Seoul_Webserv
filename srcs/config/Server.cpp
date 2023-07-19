#include "../../includes/config/Server.hpp"

Server::Server(): mRoot("/"), mListen(80), mClientMaxBodySize(MAX_BODY_SIZE) { }

void	Server::setValue(const vector<string>& temp)
{
	if (temp[0] == "error_page" && temp.size() > 2) {
		for (size_t i = 1; i < temp.size() - 1; i++) {
			this->addErrorPage(temp[i], temp[temp.size() - 1]);
		}
		return ;
	} else if (temp[0] == "server_name") {
		for (size_t i = 1; i < temp.size(); i++) {
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
		vector<string> temp = SpiderMenUtil::splitString(line);

		if (temp.size() == 0 || temp[0][0] == '#')
			continue;
		if (temp[0][0] == '}' && temp.size() == 1) {
			return ;//end parsing
		} else if (temp.size() < 2) {
			throw runtime_error("Error: Invalid server: key - value");
		} else if (temp[0] == "location" && temp[2] == "{" && temp.size() == 3) {
			Location location(temp[1]);
			location.parse(f_dataRead);
			this->addLocation(location);
		} else {
			if (!(line.length() - 1 == line.find(';')))
				throw runtime_error("Error: Invalid location: ';'");
				
			line = line.substr(0, line.length() - 1);
			temp = SpiderMenUtil::splitString(line);
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
	if (mListen.size() > 5)
		throw runtime_error("Error: too big listen port");
	for(size_t i = 0; i < mListen.size(); i++)
	{
		cout << mListen << endl;
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
	for (size_t j = 0; j < this->getLocation().size(); ++j)
	{
		cout << "	Location [" << this->getLocation()[j].getKey() << "] --------------\n";
		this->getLocation()[j].printMembers();
	}
}
