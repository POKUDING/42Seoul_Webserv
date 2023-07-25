#include "../../includes/request/RGet.hpp"

RGet::RGet(string mRoot, map<string, string> header_key_val)
			: ARequest(mRoot, nMethod::GET, header_key_val)
{
	if (mBasics.content_length || mBasics.transfer_encoding.size()) {
		throw runtime_error("Bad request:: GET cannot have body");
	}
}
			
RGet::~RGet() { }

const string	RGet::createResponse()
{
	string	mMSG;
	char 	timeStamp[TIME_SIZE];
	stringstream to_str;
	stringstream tmp;
	string		buffer;
	string		body;

	//요청받은 파일 크기 계산 및 BODY용 stream 처리 (default:index.htmp)
	ifstream	fin("./www/index.html");
	if (fin.fail())
		throw runtime_error("Error: GET body file read failed");
	tmp << fin.rdbuf();
	body = tmp.str();
	fin.close();

	//1st line: STATUS
	mMSG.append(HTTP_STATUS);	//"HTTP/1.1 200 OK\r\n"
	
	//HEADER============================================
	Time::stamp(timeStamp);
	mMSG.append(timeStamp);		//Date: Tue, 20 Jul 2023 12:34:56 GMT\r\n
	mMSG.append(SPIDER_SERVER);	//Server: SpiderMen/1.0.0\r\n

	mMSG.append(CONTENT_TYPE);	//Content-Type: text/html; charset=UTF-8\r\n
	// mMSG.append("Content-Type: ");	//png 등의 경우 별도의 content-type필요
	
	mMSG.append("Content-Length: ");
	cout << "body size = " << body.size() << endl;
	to_str << body.size();
	to_str >> buffer;
	cout << "body size  in buffer = " << buffer << endl;
	mMSG.append(buffer);
	mMSG.append("\r\n");

	if (this->getBasics().connection == nSocket::KEEP_ALIVE)
		mMSG.append("Connection: Keep-Alive\r\n");

	mMSG.append("\r\n"); //end of head

	//BODY 추가
	mMSG.append(body.c_str(), body.size());
	return mMSG;
}
