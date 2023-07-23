#include "../../includes/request/ARequest.hpp"

ARequest::~ARequest() { }

ARequest::ARequest(string mRoot, int mType, map<string, string> header_key_val)
			: mRoot(mRoot), mType(mType)
{
	mBasics.host = header_key_val["Host"];
	mBasics.connection = header_key_val["Connection"];
	mBasics.user_agent = header_key_val["User-Agent"];
	mBasics.accept = header_key_val["Accept"];
	mBasics.accept_encoding = header_key_val["Accept-Encoding"];
	mBasics.accept_language = header_key_val["Accept-Language"];

	//s_basic에서 필수적인 부분이 충족되지 않았으면 => error
}

int					ARequest::getType() const { return mType; }
const string&		ARequest::getRoot() const { return mRoot; }
const t_basic&		ARequest::getBasics() const { return mBasics; }
const t_response&	ARequest::getResponse() const { return mResponse; }
const string&		ARequest::getMSG() const { return mMSG; }

void				ARequest::setMSG(const string& msg) { mMSG.append(msg); }