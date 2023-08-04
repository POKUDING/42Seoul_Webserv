#include "../../includes/socket/Client.hpp"

// constructor and destructor

Client::Client(bool mType, int mFd, int mPort, vector<Server>* mServer, KQueue& mKq)
			: Socket(mType, mFd, mPort, mServer, mKq), mReadStatus(WAITING), mRequestStatus(EMPTY),mResponseCode(0), mPid(0), mWriteLast(0)
{
	setReadLast();	//read 시간 초기화
}

Client::~Client()
{
	// cout << "\n\n 소멸자자자 " << mRequests.size() << endl;
	for (size_t i = 0, end = mRequests.size(); i < end; ++i)
	{
		delete mRequests.front();
		mRequests.pop();
	}
}

// member functions

// public

// handler
void			Client::handleClientRead(struct kevent* event)
{
	if (mRequests.size() != 0 && static_cast<int>(event->ident) == mRequests.front()->getReadPipe())
		readPipe(event); //read pipe value;
	else
		readSocket(event);
}

void			Client::handleClientWrite(struct kevent* event)
{
	if (mRequests.size() != 0 && static_cast<int>(event->ident) == mRequests.front()->getWritePipe())
		writePipe(event);//write pipe;
	else
		writeSocket(event);
}

void			Client::handleProcess(struct kevent* event)
{
	int	exit_status = 0;

	waitpid(event->ident, &exit_status, 0);
	// cout << "handle process: " << exit_status <<endl;

	// cout << mRequests.front()->getPipeValue() << "$" << endl;

	// mRequests.front()->checkPipe();
	
	// cout << mRequests.front()->getPipeValue() << endl;

	if (exit_status != EXIT_SUCCESS)
	{
		// cerr << "fork exit with 1 " << endl;
		throw 500;
	}
	// 	mRequests.front()->setCode(500);
	// else
	// 	mRequests.front()->setCode(0);
}

// readfunc

void	Client::readSocket(struct kevent* event)
{
	size_t	buffer_size = event->data;
	char	buffer[buffer_size];
	
	memset(buffer, 0, buffer_size);
	while (1)
	{
		ssize_t s = recv(getFd(), buffer, buffer_size, 0);
		if (s < 1) {
			if (s == 0) {
				// cerr << "socket closed" << endl;
				throw 0;
			} else 
				break;
		}
		mInputBuffer.append(buffer, s);
	}
	// cout << "recv buffer: " << mInputBuffer << endl;
	while (addBuffer())
	{
		if (mHeader.getHeadBuffer().size())
			addRequests(createRequest(mHeader));
	}
}

int			Client::addBuffer()
{
	if (mReadStatus <= READING_HEADER)	{
		mReadStatus = READING_HEADER;
		return mHeader.addHead(mInputBuffer);
	} else if (mReadStatus == READING_BODY && mRequests.back()->getBody().addBody(mInputBuffer)) {
		// cout << "requests Type: " << mRequests.back()->getType() << endl;		
		if (mRequests.back()->getBody().getMaxBodySize() < mRequests.back()->getBody().getSize())
		{
			// cerr << "in Len body over than maxbody size Error" << endl;
			throw 413;
		}
		if (mRequests.back()->getType() == BAD)
			mReadStatus = ERROR;
		else
			mReadStatus = WAITING;
		if (mRequests.size() == 1) {
			// cout << "--> call operate request 2" << endl;
			operateRequest(mRequests.front());
		}
		return 1;
	}
	return 0;
}

void	Client::readPipe(struct kevent* event)
{
	char	buff[event->data];
	string	readvalue;
	int		readlen;

	// cout << "start" << endl;
	while (1)
	{
		readlen = read(event->ident, buff, event->data);
		if (readlen < 0) {
			break;
		}

		// cout << "fin, buff: " << buff << endl;
		
		if (readlen == 0) {
			close (event->ident);//read pipe close;
			mRequests.front()->getPipeValue() = SpiderMenUtil::replaceCRLF(mRequests.front()->getPipeValue());
			if (mRequests.front()->getPipeValue().find("Content-Type:") == string::npos)
			{
				size_t pos = mRequests.front()->getPipeValue().find("\r\n\r\n");
				if (pos == string::npos)
					mRequests.front()->getPipeValue() = "\r\n" + mRequests.front()->getPipeValue();
				mRequests.front()->getPipeValue() = "Content-Type: text/plain; charset=UTF-8\r\n" + mRequests.front()->getPipeValue();
			}
			// cout << mRequests.front()->getPipeValue() << "$ if 문 안쪽 "<<endl;
			// cout << "find \\r\\n" << mRequests.front()->getPipeValue().find("\r\r\n", 17) << ": " << mRequests.front()->getPipeValue().substr(mRequests.front()->getPipeValue().find("\r\r\n", 17)) << endl;
			mRequestStatus = SENDING;
			break;
		}
		mRequests.front()->getPipeValue().append(buff, readlen);
	}
}

//requests func

void			Client::addRequests(ARequest* request)
{
	// cout <<"--------in Add Request: chunked: " << request->getBody().getChunked() << endl;
	mRequests.push(request);
	if (request->getType() == POST)
		mReadStatus = READING_BODY;
	else
		mReadStatus = WAITING;
	if (mReadStatus == WAITING && mRequests.size() == 1) {
		// cout << "--> call operate request 1" << endl;
		operateRequest(mRequests.front());
	}
}

ARequest*	Client::createRequest(Head& head)
{
	vector<string>		element_headline;
	map<string,string>	header_key_val;
	
	cout << "\n\n\nNew request ========\n" << head.getHeadBuffer() << endl;
	string header = head.getHeadBuffer();
	try {
		vector<string>		header_line;
		
		header_line = SpiderMenUtil::splitString(header, "\r\n");
		header_key_val = createHttpKeyVal(header_line);
		element_headline = SpiderMenUtil::splitString(header_line[0]);
		head.clear();
		if (count(header_line[0].begin(), header_line[0].end(), ' ') != 2 || element_headline.size() != 3) {
			// cerr << "create Request headline Error" << endl;
			throw 400;
		}
		if (element_headline[2] != "HTTP/1.1") {
			throw 505;
		}
		if (element_headline[0] == "GET")
			return new RGet(element_headline[1], header_key_val, mServer);
		else if (element_headline[0] == "POST" || element_headline[0] == "PUT")
			return new RPost(element_headline[1], header_key_val, mServer);
		else if (element_headline[0] == "DELETE")
			return new RDelete(element_headline[1], header_key_val, mServer);
		else {
			throw 405;
		}
	} catch (int error) {
		if (element_headline[0] == "POST")
			mReadStatus = READING_BODY;
		else
			mReadStatus = ERROR;
		if (element_headline[0] == "HEAD")
			return new RBad(error, header_key_val, mServer, HEAD);
		return new RBad(error, header_key_val, mServer);
	} catch(const std::exception& e) {
		cout << "UNEXPECTED Error in ARequest: " << e.what() << endl;
		return new RBad(400, header_key_val, mServer);
	}
}

map<string,string>	Client::createHttpKeyVal(const vector<string>& header_line)
{
	map<string, string>	rtn;
	size_t				pos;
	
	for (size_t i = 1, end = header_line.size(); i < end; ++i)
	{
		pos = header_line[i].find(": ");
		if (pos == string::npos)
			throw runtime_error("Error: invalid http" + header_line[i]);
		if (rtn[header_line[i].substr(0, pos)] == "")
			rtn[header_line[i].substr(0, pos)] = header_line[i].substr(pos + 2, header_line[i].size() - pos + 2);
		else
			throw runtime_error("Error: invalid http duplicated key");
	}
	return rtn;
}

//operate func

void			Client::operateRequest(ARequest* request)
{
	// cout << request->getBody().getBody() << endl;
	pid_t	pid = request->operate();
	
	if (pid) {
		mKq.addProcessPid(pid, reinterpret_cast<void*>(this));
		mKq.addPipeFd(mRequests.front()->getWritePipe(), mRequests.front()->getReadPipe(), reinterpret_cast<void*>(this));
		//event push;
		mRequestStatus = PROCESSING;
		// cout << "now to PROCESSING" << endl;
	} else {
		mRequestStatus = SENDING;
		// cout << "now to SENDING\n";		
	}
}

//write func

void			Client::writeSocket(struct kevent* event)
{
	if (mResponseMSG.size() == 0 && mRequests.size())
	{
		if (mRequests.front()->getCode() < 400)
			mResponseMSG = mRequests.front()->createResponse();
		else
		{
			// RBad badRequest(mRequests.front()->getCode(), mRequests.front()->getServer());
			// mResponseMSG = badRequest.createResponse();
			mResponseMSG = mRequests.front()->createResponse();
			mRequests.front()->setType(BAD);
		}
	}
	if (mRequests.size() && sendResponseMSG(event))
	{
		if (mRequests.front()->getType() == BAD) {
			// cerr << "front type is BAD" << endl;
			throw 0;
		}
		delete mRequests.front();
		mRequests.pop();
		if (!mRequests.empty()) {
			// cout << "--> call operate request 3" << endl;
			operateRequest(mRequests.front());
		} else
			mRequestStatus = EMPTY;
	}
}

int			Client::sendResponseMSG(struct kevent* event)
{
	//TEST_CODE: response msg
	// cout << "Request: " << getRequests().front()->getType() << ", dir: " << getRequests().front()->getRoot() << endl;
	if (getRequests().front()->getSendLen() == 0) {
		if (getResponseMSG().size() > 1000)
			cout << "Response+++++++++\n" << getResponseMSG().substr(0, 500) << "++++++++++++++"<< endl;
		else
			cout << "Response+++++++++\n" << getResponseMSG() << "++++++++++++++" << endl;
	}

	
	
	size_t	sendinglen = getResponseMSG().size() - getRequests().front()->getSendLen();
	if (static_cast<size_t>(event->data) <= sendinglen)
		sendinglen = event->data;
	sendinglen = send(getFd(), getResponseMSG().c_str() + getRequests().front()->getSendLen(), sendinglen, 0);
	
	//이 아래 if문 필요한지 확인 필요 (필요하면 중괄호 안 내용 수정 필요할듯합니다 => throw error)
	if (sendinglen == (size_t) -1) {
		cout << "sending len -1" << endl;
		throw 0;
	}
	
	getRequests().front()->addSendLen(sendinglen);
	// cout << "sent this time: " << sendinglen << ", already sent: " << getRequests().front()->getSendLen() << endl;
	
	if (getRequests().front()->getSendLen() == getResponseMSG().size())
	{
		cout << "send done==========" << endl;
		mResponseMSG.clear();
		return 1;
	}
	return 0;
}

void		Client::writePipe(struct kevent* event)
{
	size_t	sendingLen = event->data;
	size_t	sendLen = 0;

	if (sendingLen >= mRequests.front()->getBody().getBody().size())
		sendingLen = mRequests.front()->getBody().getBody().size();
	if (sendingLen)
		sendLen = write(event->ident,  mRequests.front()->getBody().getBody().c_str(), sendingLen);
	if (sendingLen && sendLen <= 0)
	{
		close (event->ident);
		// cerr << "input to pipe error" << endl;
		throw 500;
	}
	mRequests.front()->getBody().getBody() = mRequests.front()->getBody().getBody().substr(sendLen);
	if (mRequests.front()->getBody().getBody().size() == 0)
		close (event->ident);
}

void			Client::clearClient()
{
	mReadStatus = WAITING;
	mResponseCode = 0;
	mInputBuffer.clear();
	getInputBuffer().clear();
	mResponseMSG.clear();
}

// getters and setters

int					Client::getReadStatus() const { return mReadStatus; }
queue<ARequest*>&	Client::getRequests() { return mRequests; }
string&				Client::getResponseMSG() { return mResponseMSG; }
string&				Client::getHeadBuffer() { return mInputBuffer; }
string&				Client::getInputBuffer() { return mInputBuffer; }
int					Client::getResponseCode() const { return mResponseCode; }
pid_t				Client::getCGI() const { return mPid; }
int					Client::getRequestStatus() const { return mRequestStatus; }
std::time_t			Client::getReadLast() const { return mReadLast; }
std::time_t			Client::getWriteLast() const { return mWriteLast; }

void				Client::setReadStatus(int mStatus) { this->mReadStatus = mStatus; }
void				Client::setResponseCode(int code) { mResponseCode = code; }
void				Client::setCGI(pid_t mPid) { this->mPid = mPid; }
void				Client::setRequestStatus(int mRequestStatus) {this->mRequestStatus = mRequestStatus; }
void				Client::setReadLast() { mReadLast = Time::stamp(); }
void				Client::setWriteLast() { mWriteLast = Time::stamp(); }