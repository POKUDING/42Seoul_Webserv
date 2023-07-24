#include "../../includes/request/RDelete.hpp"

RDelete::RDelete(string mRoot, map<string, string> header_key_val)
			: ARequest(mRoot, nMethod::DELETE, header_key_val)
{
	if (mBasics.content_length || mBasics.transfer_encoding.size()) {
		throw runtime_error("Bad request:: DELETE cannot have body");
	}
}

RDelete::~RDelete() { }

const string&	RDelete::createResponse()
{
	if (mResponse.code == 400) {
		create400Response();
	} else if (mResponse.code == 500) {
		create500Response();
	} else { //if DELETE done
	
		//1st line: STATUS
		mMSG.append(HTTP_STATUS);	//"HTTP/1.1 200 OK\r\n"

		//HEADER============================================
		mMSG.append("204 No Content\r\n");
		Time::stamp(timeStamp);
		mMSG.append(timeStamp);	//Date: Tue, 20 Jul 2023 12:34:56 GMT
		mMSG.append(SPIDER_SERVER);	//Server: SpiderMen/1.0.0

		if (this->getBasics().connection == nSocket::KEEP_ALIVE)
			mMSG.append("Connection: Keep-Alive\r\n");
			
		mMSG.append("\r\n");

		//BODY 없음
	}
	return mMSG;
}