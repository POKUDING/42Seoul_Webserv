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
	int				getIdx() const;
	// int				getBuFlag() const;
	ARequest*		getRequest() const;

	void			setStatus(int mStatus);
	void			setIdx(const int mIdx);
	void			setMethod(const int method);
	// void			setBuFlag(int mBuFlag);
	void			setRequestNull();
	void			addBuffer(char *input, size_t size);

	int				createRequest(const string& header);
	map<string,string>	createHttpKeyVal(const vector<string>&	header_line);
	// int					checkRequest(const string& headline);
	// void				example();
	void			resetTimer(int mKq, struct kevent& event);

	string			mHeadBuffer;
	string			mBodyBuffer;

private:
	// void			parseHeader(void);
	ARequest*		mRequest;
	unsigned int	mTime;
	int				mStatus;	//nStatus::eClient
	int				mIdx;
	// int				mBuFlag;
	
};

#endif //CLIENT_HPP