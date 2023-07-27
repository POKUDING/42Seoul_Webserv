#include "../../includes/request/RDelete.hpp"

// constructor and destructor

RDelete::RDelete(string mRoot, map<string, string> header_key_val, vector<Server>* servers)
			: ARequest(mRoot, GET, header_key_val, servers)
{
	if (mBasics.content_length || mBasics.transfer_encoding.size())
		throw runtime_error("Bad request:: DELETE cannot have body");

	//dir or file 체크
	if (!mIsFile)
		throw runtime_error("Bad request:: DELETE:: cannot delete directory");
	//method 사용가능한지 확인
	if (find(mLocation.getLimitExcept().begin(), mLocation.getLimitExcept().end(), "DELETE") == \
			mLocation.getLimitExcept().end())
		throw runtime_error("Bad request:: DELETE:: method not available");
}

RDelete::~RDelete() { }

// member functions

// public

pid_t			RDelete::operate()
{
	setPipe();	//pipe 생성 (ARequest에 있음)

	pid_t id = fork();
	if (id == 0) {
		//Delete 가능한 location만 들어옴.
		char* const argv[2] = {const_cast<char * const>(mRoot.c_str()), NULL};
		execve("/bin/rm", argv, env);
	}
	return id;
}

const string	RDelete::createResponse()
{
	string	mMSG;
	char 	timeStamp[TIME_SIZE];

	//1st line: STATUS
	mMSG.append(HTTP_STATUS);	//"HTTP/1.1 200 OK\r\n"

	//HEADER============================================
	mMSG.append("204 No Content\r\n");
	Time::stamp(timeStamp);
	mMSG.append(timeStamp);	//Date: Tue, 20 Jul 2023 12:34:56 GMT
	mMSG.append(SPIDER_SERVER);	//Server: SpiderMen/1.0.0

	if (this->getBasics().connection == KEEP_ALIVE)
		mMSG.append("Connection: Keep-Alive\r\n");
		
	mMSG.append("\r\n");

	//BODY 없음

	return mMSG;
}