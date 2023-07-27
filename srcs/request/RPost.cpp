#include "../../includes/request/RPost.hpp"

// constructor and destructor

RPost::RPost(string mRoot, map<string, string> header_key_val, vector<Server>* servers)
			: ARequest(mRoot, GET, header_key_val, servers)
{
	if (mBasics.content_length) {
		size_t	pos;
		
		pos = mBasics.content_type.find("boundary=");
		if (pos != string::npos)
			mBasics.boundary = mBasics.content_type.substr(pos + 9, mBasics.content_type.size() - pos - 9);
	} else if (mBasics.transfer_encoding == "chunked") {
		//chunked flag 필요
		mBasics.content_length = CHUNKED;
	} else {
		throw runtime_error("Bad request:: Post:: no content-length");
	}

	//dir or file 체크
	if (mIsFile)
		throw runtime_error("Bad request:: Post:: not valid directory");
	//method 사용가능한지 확인
	if (find(mLocation.getLimitExcept().begin(), mLocation.getLimitExcept().end(), "POST") == \
			mLocation.getLimitExcept().end())
		throw runtime_error("Bad request:: Post:: method not available");
}

RPost::~RPost() { }

// member functions

// public

const string	RPost::createResponse()
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


	mMSG.append("Content-Length: 13\r\n");

	if (this->getBasics().connection == KEEP_ALIVE)
		mMSG.append("Connection: Keep-Alive\r\n");
		
	mMSG.append("\r\n");		//end of head

	//BODY============================================
	//mMSG에 요청받은 페이지 내용 추가
	mMSG.append("POST SUCCESS!\r\n");

	return mMSG;
}

// getters and setters

const string&	RPost::getBody() const { return mBody; }
void			RPost::setBody(string mBody) { this->mBody = mBody; }