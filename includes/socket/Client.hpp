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
#include "../util/Time.hpp"

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
		
		//handler
		void				handleClientRead(struct kevent* event);
		void				handleClientWrite(struct kevent* event);
		void				handleProcess(struct kevent* event);
		
		//read
		void				readSocket(struct kevent* event);
		int					addBuffer();
		void				addRequests(ARequest* request);
		ARequest*			createRequest(Head& head);
		map<string,string>	createHttpKeyVal(const vector<string>&	header_line);

		//operate
		void				operateRequest(ARequest* request);


		//write
		void				writeSocket(struct kevent* event);
		int					sendResponseMSG(struct kevent* event);

		//read-pipe
		void				readPipe(struct kevent* event);
		//write-pipe
		void				writePipe(struct kevent* event);

		//reset client
		void				clearClient();

//to amend
		// int					checkRequest(const string& headline);
		
		// void				resetTimer(int mKq, struct kevent event);

		//getter && setter
		int					getReadStatus() const;
		queue<ARequest*>&	getRequests();
		string&				getResponseMSG();
		string&				getHeadBuffer();
		string&				getInputBuffer();
		int					getResponseCode() const;
		pid_t				getCGI() const;
		int					getRequestStatus() const;
		std::time_t			getReadLast() const;
		std::time_t			getWriteLast() const;

		void				setReadStatus(int mStatus);
		void				setResponseCode(int code);
		void				setCGI(pid_t mPid);
		void				setRequestStatus(int mRequestStatus);
		void				setReadLast();
		void				setWriteLast();

	private:
		// void			parseHeader(void);
		queue<ARequest*>	mRequests;
		int					mReadStatus;	//eClient
		int					mRequestStatus; 
		int					mResponseCode;
		string				mInputBuffer;
		string				mResponseMSG;
		pid_t				mPid;
		Head				mHeader;
		time_t				mReadLast;
		time_t				mWriteLast;
};

#endif //CLIENT_HPP