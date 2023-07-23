#include "../../includes/request/RDelete.hpp"

RDelete::RDelete(string mRoot, map<string, string> header_key_val)
			: ARequest(mRoot, nMethod::DELETE, header_key_val)
{
	// parse(header_key_val);
}

RDelete::~RDelete() { }

// void	RDelete::parse(map<string, string> header_key_val)
// {

// }

const string&	RDelete::createResponse()
{
	
	stringstream	to_str;

	//1st line: STATUS
	mMSG.append(HTTP_VERSION);	//"HTTP/1.1 " (띄어쓰기 포함!)
	to_str << getResponse().code;
	to_str >> mMSG;
	mMSG.append(" ");
	mMSG.append(getResponse().status);
	mMSG.append("\r\n");

	//HEADER
	Time::stamp(timeStamp);
	mMSG.append(timeStamp);	//Date: Tue, 20 Jul 2023 12:34:56 GMT
	mMSG.append(SPIDER_SERVER);	//Server: SpiderMen/1.0.0 (MAC OS)
	mMSG.append(CONTENT_TYPE);	//Content-Type: text/html; charset=UTF-8
	mMSG.append("Content-Length: ");
	to_str << getResponse().content_length;
	to_str >> mMSG;

	//BODY



	return mMSG;
}