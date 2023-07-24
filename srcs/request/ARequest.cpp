#include "../../includes/request/ARequest.hpp"

ARequest::~ARequest() { }

ARequest::ARequest(string mRoot, int mType, map<string, string> header_key_val)
			: mRoot(mRoot), mType(mType)
{
	mBasics.host = header_key_val["Host"];
	mBasics.user_agent = header_key_val["User-Agent"];
	mBasics.content_length = atoi(header_key_val["Content-Length"].c_str());
	mBasics.content_type = header_key_val["Content-Type"];
	mBasics.content_disposition = header_key_val["Content-Disposition"];
	mBasics.transfer_encoding = header_key_val["Transfer-encoding"];

	//method 필수요소 확인: host, user_agent
	if (mBasics.host.size() == 0 || mBasics.user_agent.size() == 0)
		throw runtime_error("Bad request:: no must header");
	//method 필수요소 확인: connection
	if (header_key_val["Connection"] == "keep-alive") {
		mBasics.connection = nSocket::KEEP_ALIVE;
	} else if (header_key_val["Connection"] == "close") {
		mBasics.connection = nSocket::CLOSE;
	} else {
		throw runtime_error("Bad request:: connection");
	}
	//method 필수요소 확인: content_length 확인 (숫자인지 아닌지 여부)
	if (mBasics.content_length == 0 && header_key_val["Content-Length"].size()) {
		throw runtime_error("Bad request:: content-length");
	}

	//필요해서 초기화해줌 (struct가 자동으로 초기화되면 필요없음 근데 잘 모름ㅎ)
	mResponse.code = 0;
	mResponse.content_length = 0;
}

int					ARequest::getType() const { return mType; }
const string&		ARequest::getRoot() const { return mRoot; }
const t_basic&		ARequest::getBasics() const { return mBasics; }
const t_response&	ARequest::getResponse() const { return mResponse; }
const string&		ARequest::getMSG() const { return mMSG; }

void				ARequest::setMSG(const string& msg) { mMSG.append(msg); }
void				ARequest::setResponse(int code, const string& status, int len)
{
	mResponse.code = code;
	mResponse.status = status;
	mResponse.content_length = len;
}

void				ARequest::create400Response()
{
	mMSG.append("HTTP/1.1 400 Bad Request\r\n");
	mMSG.append("Content-Type: text/plain\r\n");
	mMSG.append("Content-Length: 14\r\n");
	mMSG.append("Bad Request :(");
}

void				ARequest::create500Response()
{
	mMSG.append("HTTP/1.1 500 Internal Server Error\r\n");
	mMSG.append("Content-Type: text/plain\r\n");
	mMSG.append("Content-Length: 25\r\n");
	mMSG.append("Internal Server Error :'(");
}