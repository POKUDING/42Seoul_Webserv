#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sstream>
#include <sys/event.h>
#include "../request/ARequest.hpp"
#include "../request/RGet.hpp"
#include "../request/RPost.hpp"
#include "../request/RDelete.hpp"
#include "./ASocket.hpp"

//REQUEST STRUCT
	// method
	// root
	// http
	// Connection
	// Accept
	// host
	// Content-Length
	// transfer-coding
	//Content-Type
		//boundary
		//Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryf4QX1ApB5cA72Bt7



class Client : public ASocket
{
public:
	Client(bool mType, int mFd, int mPort, const vector<Server>* mServer);
	virtual ~Client();

	int				getStatus() const;
	ARequest*		getRequest() const;
	string&			getResponseMSG();
	string&			getHeadBuffer();
	string&			getBodyBuffer();
	int				getResponseCode() const;

	void			setStatus(int mStatus);
	void			setRequestNull();
	void			addBuffer(char *input, size_t size);
	void			setResponseCode(int code);

	void			clearClient();

	int				createRequest(const string& header);
	map<string,string>	createHttpKeyVal(const vector<string>&	header_line);
	// int					checkRequest(const string& headline);
	// void				example();
	void			resetTimer(int mKq, struct kevent event);

	void			create400Response();
	void			create500Response();


private:
	// void			parseHeader(void);
	ARequest*		mRequest;
	int				mStatus;	//nStatus::eClient
	int				mResponseCode;
	string			mHeadBuffer;
	string			mBodyBuffer;
	string			mResponseMSG;
};

#endif //CLIENT_HPP