#include "../../includes/config/Location.hpp"

// constructor
Location::Location() { }
Location::Location(const string& mKey): mKey(mKey), mAutoIndex(false) { }

// member functions

void	Location::parse(ifstream& f_dataRead)
{
	string line;
    while (getline(f_dataRead, line)){

		if (line.length() == 0)
			continue;

		vector<string> splitedLine = SpiderMenUtil::splitString(line);
		if (splitedLine.size() == 0 || splitedLine[0][0] == '#')
			continue;
		if (splitedLine[0][0] == '}' && splitedLine.size() == 1) {
			//limit_except 가 없을 경우 get, post, delete 추가
			if (getLimitExcept().size() == 0)
				mLimitExcept.push_back("GET");
			for (int i = 0, end = mLimitExcept.size(); i < end; ++i) {
				if (mLimitExcept[i] == "POST" && (!mCgiBin.size() || !mCgiPath.size()))
					throw runtime_error("Error: Invalid location: POST need cgi");
			}
			return ;//end parsing
		} else if (splitedLine.size() < 2) {
			throw runtime_error("Error: Invalid location: too less value");
		} else {
			if (!(line.length() - 1 == line.find(';')))
				throw runtime_error("Error: Invalid location: ';'");
			
			line = line.substr(0, line.length() - 1);
			splitedLine = SpiderMenUtil::splitString(line);
			setValue(splitedLine);
		}
	}
}

void	Location::setValue(const vector<string>& splitedLine)
{
	if (splitedLine[0] == "limit_except") {
		for (size_t i = 1; i < splitedLine.size(); i++) {
			this->addLimitExcept(splitedLine[i]);
		}
		return ;
	} else if (splitedLine.size() != 2) {
		throw runtime_error("Error: Invalid location: too many values");
	}

	if (splitedLine[0] == "root") {
		this->setRoot(splitedLine[1]);
	} else if (splitedLine[0] == "redirection") {
		this->setRedirect(splitedLine[1]);
	} else if (splitedLine[0] == "cgi_bin") {
		this->setCgiBin(splitedLine[1]);
	} else if (splitedLine[0] == "cgi_path") {
		this->setCgiPath(splitedLine[1]);
	} else if (splitedLine[0] == "index") {
		this->setIndex(splitedLine[1]);
	} else if (splitedLine[0] == "autoindex") {
		this->setAutoIndex(splitedLine[1]);
	} else if (splitedLine[0] == ".sh") {
		this->setSh(splitedLine[1]);
	} else if (splitedLine[0] == ".py") {
		this->setPy(splitedLine[1]);
	} else if (splitedLine[0] == ".php") {
		this->setPhp(splitedLine[1]);
	} else if (splitedLine[0] == "return") {
		this->setReturn(splitedLine[1]);
	} else {
		throw runtime_error("Error: Invalid location: no such key");
	}
}

// getters and setters

const string&			Location::getKey() const { return this->mKey; }
const string&			Location::getRoot() const { return this->mRoot; }
const vector<string>&	Location::getLimitExcept() const { return this->mLimitExcept; }
const string&			Location::getCgiBin() const { return this->mCgiBin; }
const string&			Location::getCgiPath() const { return this->mCgiPath; }
const string&			Location::getIndex() const { return this->mIndex; }
bool					Location::getAutoIndex() const { return this->mAutoIndex; }
const string&			Location::getSh() const { return this->mSh; }
const string&			Location::getPy() const { return this->mPy; }
const string&			Location::getPhp() const { return this->mPhp; }
const string&			Location::getReturn() const { return this->mReturn; }
const string&			Location::getRedirect() const { return this->mRedirect; }

void					Location::setRoot(const string& mRoot) { this->mRoot = mRoot; }
void					Location::addLimitExcept(const string& mLimitExcept)
{
	if (mLimitExcept == "GET" || mLimitExcept == "POST" || mLimitExcept == "DELETE")
		this->mLimitExcept.push_back(mLimitExcept);
	else
		throw runtime_error("Error: Invalid location: Limit Except");
}
void					Location::setCgiBin(const string& mCgiBin) { this->mCgiBin = mCgiBin; }
void					Location::setCgiPath(const string& mCgiPath) { this->mCgiPath = mCgiPath; }
void					Location::setIndex(const string& mIndex) { this->mIndex = mIndex; }
void					Location::setRedirect(const string& mRedircect ) { this->mRedirect = mRedircect; }
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

// print

void	Location::printMembers() const
{
	cout << "		LimitExcept: \n";
	for (size_t k = 0; k < this->getLimitExcept().size(); ++k)
		cout << "\t\t\t" << k << ": " << this->getLimitExcept()[k] << "\n";
	cout << "		Root: " << this->getRoot() << "\n";
	cout << "		CgiPath: " << this->getCgiPath() << "\n";
	cout << "		Index: " << this->getIndex() << "\n";
	cout << "		AutoIndex: " << this->getAutoIndex() << "\n";
	cout << "		Sh: " << this->getSh() << "\n";
	cout << "		Py: " << this->getPy() << "\n";
	cout << "		Php: " << this->getPhp() << "\n";
	cout << "		Sh: " << this->getReturn() << "\n\n";
}
