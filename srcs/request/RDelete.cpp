#include "../../includes/request/RDelete.hpp"

// constructor and destructor

RDelete::RDelete(string mRoot, map<string, string> header_key_val, vector<Server>* servers)
			: ARequest(mRoot, DELETE, header_key_val, servers)
{
	if (mBasics.content_length || mBasics.transfer_encoding.size()) {
		mCode = 400;
		throw runtime_error("Bad request:: DELETE cannot have body");
	}

	//dir or file 체크
	if (!mIsFile) {
		mCode = 400;
		throw runtime_error("Bad request:: DELETE:: cannot delete directory");
	}
	//method 사용가능한지 확인
	if (find(mLocation.getLimitExcept().begin(), mLocation.getLimitExcept().end(), "DELETE") == \
			mLocation.getLimitExcept().end()) {
		mCode = 405;
		throw runtime_error("Bad request:: DELETE:: method not available");
	}
}

RDelete::~RDelete() { }

// member functions

// public

pid_t			RDelete::operate()
{
	// 파일 삭제 시도
    if (remove(getRoot().c_str()) == 0) {//success 
		mCode = 200;
    } else {//권한 없어 실패
		mCode = 405;
    }
	return 0;
}

const string	RDelete::createResponse()
{
	string	mMSG;
	char 	timeStamp[TIME_SIZE];

	//1st line: STATUS
	mMSG.append(HTTP_STATUS);	//"HTTP/1.1 200 OK\r\n"
	
	//HEADER============================================
	Time::stamp(timeStamp);
	mMSG.append(timeStamp);	//Date: Tue, 20 Jul 2023 12:34:56 GMT\r\n
	mMSG.append(SPIDER_SERVER);	//Server: SpiderMen/1.0.0\r\n

	mMSG.append(CONTENT_TYPE);	//Content-Type: text/html; charset=UTF-8\r\n
	// mMSG.append("Content-Type: ");	//png 등의 경우 별도의 content-type필요


	mMSG.append("Content-Length: 15\r\n");

	if (this->getBasics().connection == KEEP_ALIVE)
		mMSG.append("Connection: Keep-Alive\r\n");
		
	mMSG.append("\r\n");		//end of head

	//BODY============================================가
	mMSG.append("DELETE SUCCESS!\r\n");

	return mMSG;
}