#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sstream>
#include "../request/ARequest.hpp"
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

	unsigned int	getTime() const;
	int				getStatus() const;
	int				getIdx() const;

	void			setTime(const unsigned int mTime);
	void			setStatus(const int mStatus);
	void			setIdx(const int mIdx);
	void			setMethod(const int method);
	void			addBuffer(char *input, size_t size);

	void			example();

	std::string		mHeadBuffer;
	std::string		mBodyBuffer;
	int				mBuFlag;
private:
	void			parseHeader(void);
	unsigned int	mTime;
	int				mMethod;
	int				mStatus;
	int				mIdx;
	
};

#endif //CLIENT_HPP