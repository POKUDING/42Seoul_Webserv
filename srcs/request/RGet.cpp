#include "../../includes/request/RGet.hpp"

RGet::RGet(string mRoot, map<string, string> header_key_val)
			: ARequest(mRoot, nMethod::GET, header_key_val)
{
	// parse(header_key_val);
}
			
RGet::~RGet() { }

const string&	RGet::createResponse()
{
	
	stringstream	to_str;

	//1st line: STATUS
	mMSG.append(HTTP_VERSION);	//"HTTP/1.1 " (띄어쓰기 포함!)

	// to_str << getResponse().code;
	// to_str >> response;
	mMSG.append("200");

	mMSG.append(" ");

	// mMSG.append(getResponse().status);
	mMSG.append("OK");

	mMSG.append("\r\n");

	//HEADER
	Time::stamp(timeStamp);
	mMSG.append(timeStamp);	//Date: Tue, 20 Jul 2023 12:34:56 GMT\r\n
	mMSG.append(SPIDER_SERVER);	//Server: SpiderMen/1.0.0 (MAC OS)\r\n
	mMSG.append(CONTENT_TYPE);	//Content-Type: text/html; charset=UTF-8\r\n
	mMSG.append("Content-Length: ");

	mMSG.append("225\r\n");
	// to_str << getResponse().content_length;
	// to_str >> response;

	//이거 없어서 클라가 연결 끊는건 아닌가하여 넣어봄 re:오..! 굿!!
	mMSG.append("Connection: Keep-Alive\r\n");

	mMSG.append("\r\n");		//end of head
	//BODY	//hello.html
	mMSG.append("<!DOCTYPE html>\r\n");
	mMSG.append("<html lang=\"en\">\r\n");
	mMSG.append("<head>\r\n");
	mMSG.append("    <meta charset=\"UTF-8\">\r\n");
	mMSG.append("    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n");
	mMSG.append("    <title>Hello</title>\r\n");
	mMSG.append("</head>\r\n");
	mMSG.append("<body>\r\n");
	mMSG.append("    <h1>HELLO, GETld!</h1>\r\n");
	mMSG.append("</body>\r\n");
	mMSG.append("</html>\r\n");
	mMSG.append("\r\n");		//end of body

	return mMSG;
}

// void	RGet::parse(map<string, string> header_key_val)
// {

// }