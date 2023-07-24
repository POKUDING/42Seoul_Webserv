#include "../../includes/request/RGet.hpp"

RGet::RGet(string mRoot, map<string, string> header_key_val)
			: ARequest(mRoot, nMethod::GET, header_key_val)
{
	if (mBasics.content_length || mBasics.transfer_encoding.size()) {
		throw runtime_error("Bad request:: GET cannot have body");
	}
}
			
RGet::~RGet() { }

const string&	RGet::createResponse()
{
	if (mResponse.code == 400) {
		create400Response();
	} else if (mResponse.code == 500) {
		create500Response();
	} else {//if okay
		stringstream to_str;

		//1st line: STATUS
		mMSG.append(HTTP_STATUS);	//"HTTP/1.1 200 OK\r\n"
		
		//HEADER============================================
		Time::stamp(timeStamp);
		mMSG.append(timeStamp);		//Date: Tue, 20 Jul 2023 12:34:56 GMT\r\n
		mMSG.append(SPIDER_SERVER);	//Server: SpiderMen/1.0.0\r\n

		mMSG.append(CONTENT_TYPE);	//Content-Type: text/html; charset=UTF-8\r\n
		// mMSG.append("Content-Type: ");	//png 등의 경우 별도의 content-type필요
		
		//요청받은 파일 크기 계산 필요
		mMSG.append("Content-Length: ");
		to_str << getResponse().content_length;
		to_str >> mMSG;
		mMSG.append("\r\n");

		if (this->getBasics().connection == nSocket::KEEP_ALIVE)
			mMSG.append("Connection: Keep-Alive\r\n");
			
		mMSG.append("\r\n");		//end of head

		//BODY============================================
		//mMSG에 요청받은 페이지 내용 추가

	}

	return mMSG;
}
