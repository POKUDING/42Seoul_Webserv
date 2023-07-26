#include "../../includes/request/RBad.hpp"

RBad::RBad(int code): ARequest(BAD) { createErrorResponse(code); }

RBad::~RBad() { }


const string	RBad::createResponse() { return mMSG; }

void			RBad::createErrorResponse(int code)
{
	char			timeStamp[TIME_SIZE];
	stringstream	to_str;
	stringstream	tmp;
	string			buffer;
	string			body;

	if (code == 400) {

		mMSG.append("HTTP/1.1 400 Bad Request\r\n");

		ifstream	fin("./www/errors/40x.html");
		if (fin.fail())
			throw runtime_error("Error: 400 response msg failed");
		tmp << fin.rdbuf();
		body = tmp.str();
		fin.close();

	} else if (code == 500) {

		mMSG.append("HTTP/1.1 500 Internal Server Error\r\n");
		
		ifstream	fin("./www/errors/50x.html");
		if (fin.fail())
			throw runtime_error("Error: 500 response msg failed");
		tmp << fin.rdbuf();
		body = tmp.str();
		fin.close();

	}

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