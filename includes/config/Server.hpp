#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <iostream>
#include "Location.hpp"
#include "../basic.hpp"
#include "../util/SpiderMenUtil.hpp"

using namespace std;

class Server
{
	public:
		Server();

		// location parsing related function to be added
		void						parse(ifstream& f_dataRead);
		void						setValue(const vector<string>& temp);

		// getters and setters
		const vector<Location>&		getLocation() const;
		const vector<string>&		getServerName() const;
		const map<string, string>&	getErrorPage() const;
		const string&				getRoot() const;
		int							getListen() const;
		int							getClientMaxBodySize() const;

		void						addLocation(const Location& mLocation);
		void						addServerName(const string& mServerName);
		void						addErrorPage(const string& code, const string& page);
		void						setRoot(const string& mRoot);
		void						setListen(const string& mListen);
		void						setClientMaxBodySize(const string& mClientMaxBodySize);

		void						printMembers() const;

	private:
		vector<Location>	mLocation;
		vector<string>		mServerName;
		map<string, string>	mErrorPage;
		string				mRoot;
		int					mListen;
		int					mClientMaxBodySize;	
};

template<typename K, typename V>
void printMap(map<K, V> tmap)
{
    for (typename map<K, V>::iterator it = tmap.begin(); it != tmap.end(); it++) {
        cout << "{" << (*it).first << ": " << (*it).second << "}\n";
    }
}

#endif