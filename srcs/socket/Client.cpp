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

void	Client::readSocket(struct kevent* event)
{
	size_t	buffer_size = event->data;
	char	buffer[buffer_size];
	
	memset(buffer, 0, buffer_size);
	ssize_t s = recv(getFd(), buffer, buffer_size, 0);
	
	if (s < 1) {
		if (s == 0) {
			throw runtime_error("client socket closed");
		} else {
			throw runtime_error("Error: client socket recv failed");
		}
	}

	mInputBuffer.append(buffer, s);

	// cout << "recv buffer: " << mInputBuffer << endl;
	
	while (addBuffer())
	{
		if (mHeader.getHeadBuffer().size())
			addRequests(createRequest(mHeader));
	}
	
}

void			Client::addRequests(ARequest* request)
{
	// cout <<"--------in Add Request: chunked: " << request->getBody().getChunked() << endl;
	mRequests.push(request);
	if (request->getType() == POST)
		mReadStatus = READING_BODY;
	else
		mReadStatus = WAITING;
	if (mReadStatus == WAITING && mRequests.size() == 1) {
		cout << "--> call operate request 1" << endl;
		operateRequest(mRequests.front());
	}

}

int			Client::addBuffer()
{
	if (mReadStatus <= READING_HEADER)	{
		mReadStatus = READING_HEADER;
		return mHeader.addHead(mInputBuffer);
	// } else if (mIsPost && mRequests.back()->getBody().addBody(mInputBuffer)) {
	// 	mIsPost = false;
	// 	mReadStatus = WAITING;
	// 	if (mRequests.size() == 1) {
	// 		cout << "--> call operate request 2.1" << endl;
	// 		operateRequest(mRequests.front());
	// 	}
	// 	return 1;
	} else if (mReadStatus == READING_BODY && mRequests.back()->getBody().addBody(mInputBuffer)) {
		cout << "requests Type: " << mRequests.back()->getType() << endl;
		if (mRequests.back()->getType() == BAD)
			mReadStatus = ERROR;
		else
			mReadStatus = WAITING;
		if (mRequests.size() == 1) {
			cout << "--> call operate request 2" << endl;
			operateRequest(mRequests.front());
		}
		return 1;
	}
	return 0;
}

ARequest*	Client::createRequest(Head& head)
{
	vector<string>		element_headline;
	
	cout << "\n\n\nNew request ========\n" << head.getHeadBuffer() << endl;
	string header = head.getHeadBuffer();
	try {
		vector<string>		header_line;
		map<string,string>	header_key_val;

		header_line = SpiderMenUtil::splitString(header, "\r\n");
		header_key_val = createHttpKeyVal(header_line);
		element_headline = SpiderMenUtil::splitString(header_line[0]);
		head.clear();
		if (count(header_line[0].begin(), header_line[0].end(), ' ') != 2 || element_headline.size() != 3) {
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
		return new RBad(error);
	} catch(const std::exception& e) {
		cout << "UNEXPECTED Error in ARequest: " << e.what() << endl;
		return new RBad(400);
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

void			Client::operateRequest(ARequest* request)
{
	// cout << request->getBody().getBody() << endl;
	pid_t	pid = request->operate();
	
	if (pid) {
		mKq.addProcessPid(pid, reinterpret_cast<void*>(this));
		mRequestStatus = PROCESSING;
		cout << "now to PROCESSING" << endl;
	} else {
		mRequestStatus = SENDING;
		cout << "now to SENDING\n";		
	}
}

void			Client::writeSocket(struct kevent* event)
{
	if (mResponseMSG.size() == 0)
	{
		if (mRequests.front()->getCode() < 400)
			mResponseMSG = mRequests.front()->createResponse();
		else
		{
			RBad badRequest(mRequests.front()->getCode());
			mResponseMSG = badRequest.createResponse();
		}
	}
	if (sendResponseMSG(event))
	{
		if (mRequests.front()->getType() == BAD)
		{
			mResponseCode = 0;
			throw runtime_error("send error response success");
		}
		delete mRequests.front();
		mRequests.pop();
		if (!mRequests.empty()) {
			cout << "--> call operate request 3" << endl;
			operateRequest(mRequests.front());
		} else
			mRequestStatus = EMPTY;
	}
}

int			Client::sendResponseMSG(struct kevent* event)
{
	cout << "Request: " << getRequests().front()->getType() << ", dir: " << getRequests().front()->getRoot() << endl;
	cout << "+++Response+++\n" << getResponseMSG() << "++++++++++++++" << endl;
	size_t	sendinglen = getResponseMSG().size() - getRequests().front()->getSendLen();
	if (static_cast<size_t>(event->data) <= sendinglen)
		sendinglen = event->data;
	cout << "total msg length: " << getResponseMSG().size() << ", already sent: " << getRequests().front()->getSendLen() << endl;
	cout << "possible to send: " << event->data << ", send this time: " << sendinglen << endl;
	sendinglen = send(getFd(), getResponseMSG().c_str() + getRequests().front()->getSendLen(), sendinglen, 0);
	if (sendinglen == (size_t)-1) {
		cout << "sending len -1" << endl;
	}
	getRequests().front()->addSendLen(sendinglen);
	cout << "sent this time: " << sendinglen << ", already sent: " << getRequests().front()->getSendLen() << endl;
	if (getRequests().front()->getSendLen() == getResponseMSG().size())
	{
		cout << "send done==========" << endl;
		mResponseMSG.clear();
		return 1;
	}
	return 0;
}

void			Client::handleProcess(struct kevent* event)
{
	int	exit_status = 0;

	waitpid(event->ident, &exit_status, 0);
	cout << "handle process: " << exit_status <<endl;

	mRequests.front()->checkPipe();
	
	cout << mRequests.front()->getPipeValue() << endl;

	if (exit_status != EXIT_SUCCESS)
		mRequests.front()->setCode(500);
	else
		mRequests.front()->setCode(0);
	mRequestStatus = SENDING;
}

void			Client::clearClient()
{
	mReadStatus = WAITING;
	mResponseCode = 0;
	mInputBuffer.clear();
	getInputBuffer().clear();
	mResponseMSG.clear();
}

// void	Client::example()
// {
// 	size_t leftpos = 0;
// 	size_t rightpos = mBodyBuffer.size() - 2;

// 	for(int i = 0; i < 4; ++i)
// 	{
// 		leftpos = mBodyBuffer.find("\r\n", leftpos + 2);
// 	}
// 	for(int i = 0; i < 2; ++i)
// 	{
// 		rightpos = mBodyBuffer.rfind("\r\n", rightpos - 2);
// 	}
// 	rightpos -= leftpos;
// 	string s = mBodyBuffer.substr(leftpos,rightpos);
// 	ofstream file;

// 	file.open("abc");
// 	if (file.is_open())
// 		file.write(s.c_str(), s.size());
// }

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