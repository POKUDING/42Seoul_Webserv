#include "config.hpp"

//Location ===============================
Location::Location() { memset(this, 0, sizeof(Location)); }

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
void					Location::addLimitExcept(const string& mLimitExcept) { this->mLimitExcept.push_back(mLimitExcept); }
void					Location::setFastcgiPass(const string& mFastcgiPass) { this->mFastcgiPass = mFastcgiPass; }
void					Location::setIndex(const string& mIndex) { this->mIndex = mIndex; }
void					Location::setAutoIndex(bool mAutoIndex) { this->mAutoIndex = mAutoIndex; }
void					Location::setSh(const string& mSh) { this->mSh = mSh; }
void					Location::setPy(const string& mPy) { this->mPy = mPy; }
void					Location::setPhp(const string& mPhp) { this->mPhp = mPhp; }
void					Location::setReturn(const string& mReturn) { this->mReturn = mReturn; }

//Server ===============================
Server::Server() { memset(this, 0, sizeof(Server)); }

const vector<Location>&		Server::getLocation() const { return this->mLocation; }
const map<string, string>&	Server::getErrorPage() const { return this->mErrorPage; }
const vector<string>&		Server::getServerName() const { return this->mServerName; }
const string&				Server::getRoot() const { return this->mRoot; }
int							Server::getListen() const { return this->mListen; }
int							Server::getClientMaxBodySize() const { return this->mClientMaxBodySize; }

void						Server::addLocation(const Location& mLocation) { this->mLocation.push_back(mLocation); }
void						Server::addErrorPage(const string& code, const string& page) { this->mErrorPage.insert({code, page}); }
void						Server::addServerName(const string& mServerName) { this->mServerName.push_back(mServerName); }
void						Server::setRoot(const string& mRoot) { this->mRoot = mRoot; }
void						Server::setListen(int mListen) { this->mListen = mListen; }
void						Server::setClientMaxBodySize(int mClientMaxBodySize) { this->mClientMaxBodySize = mClientMaxBodySize; }

//Config ===============================
Config::Config() { }

const vector<Server>&	Config::getServer() const { return this->mServer; }

void					Config::addServer(const Server& mServer) { this->mServer.push_back(mServer); }
