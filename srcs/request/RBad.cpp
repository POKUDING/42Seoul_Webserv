#include "../../includes/request/RBad.hpp"

// constructor and destructor

RBad::RBad(int code): ARequest(BAD) { createErrorResponse(code); }
RBad::~RBad() { }

// memeber functions

// public

pid_t			RBad::operate() { return 0; }
const string	RBad::createResponse() { return mMSG; }

void			RBad::createErrorResponse(int code)
{
	char			timeStamp[TIME_SIZE];
	stringstream	to_str;
	stringstream	tmp;
	string			filename;
	string			buffer;
	string			body;

	switch (code)
	{
	case 400:
		mMSG.append(STATUS_400);
		break;
	case 403:
		mMSG.append(STATUS_403);
		break;
	case 404:
		mMSG.append(STATUS_404);
		break;
	case 405:
		mMSG.append(STATUS_405);
		break;
	case 500:
		mMSG.append(STATUS_500);
		break;
	case 501:
		mMSG.append(STATUS_501);
		break;
	case 504:
		mMSG.append(STATUS_504);
		break;
	case 505:
		mMSG.append(STATUS_505);
		break;
	default:
		mMSG.append(STATUS_500);
		break;
	}

	if (mServer.getErrorPage()[SpiderMenUtil::itostr(code)].size())
	{
		filename = mServer.getRoot() + "/" +  mServer.getErrorPage()[SpiderMenUtil::itostr(code)];
		ifstream	fin(filename);
		if (fin.fail())
			throw runtime_error("Error: error response msg failed");
		tmp << fin.rdbuf();
		body = tmp.str();
		fin.close();
	}
	if (!body.size())
		body = mMSG;
	//HEADER============================================
	Time::stamp(timeStamp);
	mMSG.append(timeStamp);		//Date: Tue, 20 Jul 2023 12:34:56 GMT\r\n
	mMSG.append(SPIDER_SERVER);	//Server: SpiderMen/1.0.0\r\n
	mMSG.append(CONTENT_TYPE);	//Content-Type: text/html; charset=UTF-8\r\n
	
	mMSG.append("Content-Length: ");
	to_str << body.size();
	to_str >> buffer;
	mMSG.append(buffer);
	mMSG.append("\r\n");

	mMSG.append("\r\n"); //end of head

	//BODY 추가
	mMSG.append(body.c_str(), body.size());
}