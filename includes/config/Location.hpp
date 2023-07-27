#pragma once
#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <stdexcept>
#include <fstream>
#include <vector>
#include <iostream>
#include "../util/SpiderMenUtil.hpp"

using namespace std;

class Location
{
	public:
		Location();
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

		void					setRoot(const string& mRoot);
		void					addLimitExcept(const string& mLimitExcept);
		void					setFastcgiPass(const string& mFastcgiPass);
		void					setIndex(const string& mIndex);
		void					setAutoIndex(const string& mAutoIndex);
		void					setSh(const string& mSh);
		void					setPy(const string& mPy);
		void					setPhp(const string& mPhp);
		void					setReturn(const string& mReturn);

		void					printMembers() const;

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


#endif // LOCATION_HPP