#pragma once
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <vector>
#include <map>
#include <cstring>

using namespace std;

class Location
{
	public:
		Location();

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
		void					setAutoIndex(bool mAutoIndex);
		void					setSh(const string& mSh);
		void					setPy(const string& mPy);
		void					setPhp(const string& mPhp);
		void					setReturn(const string& mReturn);

	private:
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
		void						setListen(int mListen);
		void						setClientMaxBodySize(int mClientMaxBodySize);

	private:
		vector<Location>	mLocation;
		map<string, string>	mErrorPage;
		vector<string>		mServerName;
		string				mRoot;
		int					mListen;
		int					mClientMaxBodySize;
};


class Config
{
	public:
		Config();

		const vector<Server>&	getServer() const;	

		void					addServer(const Server& mServer);

	private:
		vector<Server>	mServer;
};

#endif //CONFIG_HPP