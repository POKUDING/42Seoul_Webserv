#pragma once
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cstring>
#include <stdexcept>
#include "ErrorHandle.hpp"
#include "WebsrvUtil.hpp"
#include "basic.hpp"

using namespace std;

class Location
{
	public:
		Location(const string& mKey);

		void					parse(ifstream& f_dataRead);
		void					setValue(const vector<string>& temp);

		const string&			getKey() const;
		const string&			getRoot() const;
		const vector<string>&	getLimitExcept() const;
		const string&			getFastcgiPass() const;
		const string&			getIndex() const;
		bool					getAutoIndex() const;
		const string&			getSh() const;
		const string&			getPy() const;
		const string&			getPhp() const;
		const string&			getReturn() const;

		void					addLimitExcept(const string& mLimitExcept);
		void					setRoot(const string& mRoot);
		void					setFastcgiPass(const string& mFastcgiPass);
		void					setIndex(const string& mIndex);
		void					setAutoIndex(const string& mAutoIndex);
		void					setSh(const string& mSh);
		void					setPy(const string& mPy);
		void					setPhp(const string& mPhp);
		void					setReturn(const string& mReturn);

	private:
		string			mKey;
		vector<string>	mLimitExcept;
		string			mRoot;
		string			mFastcgiPass;
		string			mIndex;
		bool			mAutoIndex;
		string			mSh;
		string			mPy;
		string			mPhp;
		string			mReturn;
};

class Server
{
	public:
		Server();

		//location parsing related function to be added
		void						parse(ifstream& f_dataRead);
		void						setValue(const vector<string>& temp);

		const vector<Location>&		getLocation() const;
		const map<string, string>&	getErrorPage() const;
		const vector<string>&		getServerName() const;
		const string&				getRoot() const;
		int							getListen() const;
		int							getClientMaxBodySize() const;

		void						addLocation(const Location& mLocation);
		void						addErrorPage(const string& code, const string& page);
		void						addServerName(const string& mServerName);
		void						setRoot(const string& mRoot);
		void						setListen(const string& mListen);
		void						setClientMaxBodySize(const string& mClientMaxBodySize);

	private:
		vector<Location>	mLocation;
		// map<string, string>	mErrorPage;
		vector<string>		mServerName;
		string				mRoot;
		int					mListen;
		int					mClientMaxBodySize;
		map<string, string>	mErrorPage;
};


class Config
{
	public:
		Config();

		enum e_keys
		{
			SERVER = 0,
			EMPTYLINE = 1,
			COMMENT = 2,
			ERROR = 3
		};

		//Config parsing related function to be added
		void					parse(const string& file = DEFAULT_FILE);
		void					printMembers();

		const vector<Server>&	getServer() const;	

		void					addServer(Server Server);
		

	private:
		vector<Server>	mServer;
};


template<typename K, typename V>
void printMap(map<K, V> tmap)
{
    for (typename map<K, V>::iterator it = tmap.begin(); it != tmap.end(); it++) {
        cout << "{" << (*it).first << ": " << (*it).second << "}\n";
    }
}


#endif //CONFIG_HPP
