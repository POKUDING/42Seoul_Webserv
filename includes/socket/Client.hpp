#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sstream>
#include <queue>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include "../kqueue/KQueue.hpp"
#include "../request/ARequest.hpp"
#include "../request/RBad.hpp"
#include "../request/RGet.hpp"
#include "../request/RPost.hpp"
#include "../request/RDelete.hpp"
#include "./Socket.hpp"
#include "./Head.hpp"
#include "./Body.hpp"

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

class Client : public Socket
{
	public:
		Client(bool mType, int mFd, int mPort, vector<Server>* mServer, KQueue& mKq);
		virtual ~Client();

		void				readSocket(struct kevent* event);
		void				addRequests(ARequest* request);
		int					addBuffer();
		void				clearClient();

		ARequest*			createRequest(Head& head);
		map<string,string>	createHttpKeyVal(const vector<string>&	header_line);

		void				writeSocket(struct kevent* event);
		int					sendResponseMSG(struct kevent* event);

		void				operateRequest(ARequest* request);

		void				handleProcess(struct kevent* event);
		// int					checkRequest(const string& headline);
		// void				example();
		void				resetTimer(int mKq, struct kevent event);

		void				createErrorResponse();

		int					getReadStatus() const;
		queue<ARequest*>	getRequests() const;
		string&				getResponseMSG();
		string&				getHeadBuffer();
		string&				getInputBuffer();
		int					getResponseCode() const;
		pid_t				getCGI() const;
		int					getRequestStatus() const;

		void				setReadStatus(int mStatus);
		// void				setRequestNull();
		void				setResponseCode(int code);
		void				setCGI(pid_t mCGI);

	private:
		// void			parseHeader(void);
		queue<ARequest*>	mRequests;
		int					mReadStatus;	//eClient
		int					mRequestStatus;
		int					mResponseCode;
		string				mInputBuffer;
		string				mResponseMSG;
		pid_t				mCGI;
		Head				mHeader;
};

#endif //CLIENT_HPP