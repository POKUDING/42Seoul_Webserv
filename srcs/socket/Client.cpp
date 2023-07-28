#include "../../includes/socket/Client.hpp"

// constructor and destructor

Client::Client(bool mType, int mFd, int mPort, vector<Server>* mServer, KQueue& mKq)
			: Socket(mType, mFd, mPort, mServer, mKq), mReadStatus(WAITING) ,mResponseCode(0), mCGI(0) { }

Client::~Client()
{
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
	while (addBuffer())
	{
		addRequests(createRequest(mHeader));
		
		//자투리 buffer 처리 필요
	}
	
}

void			Client::addRequests(ARequest* request)
{
	mRequests.push(request);
	if (request->getType() == POST)
		mReadStatus = READING_BODY;
	else
		mReadStatus = WAITING;
	
	if (mReadStatus == WAITING && mRequests.size() == 1) {
		operateRequest(mRequests.front());
	}
}

int			Client::addBuffer()
{
	if (mReadStatus <= READING_HEADER)	{
		mReadStatus = READING_HEADER;
		return mHeader.addHead(mInputBuffer);
	}
	if (mRequests.back()->getBody().addBody(mInputBuffer))
	{
		mReadStatus = WAITING;
		if (mRequests.size() == 1)
			operateRequest(mRequests.front());
	}
	return 0;
}

ARequest*	Client::createRequest(Head& head)
{
	string header = head.getHeadBuffer();
	try
	{
		vector<string>		header_line;
		vector<string>		element_headline;
		map<string,string>	header_key_val;

		header_line = SpiderMenUtil::splitString(header, "\r\n");
		header_key_val = createHttpKeyVal(header_line);
		element_headline = SpiderMenUtil::splitString(header_line[0]);
		head.clear();
		if (count(header_line[0].begin(), header_line[0].end(), ' ') != 2 || element_headline.size() != 3 || element_headline[2] != "HTTP/1.1") {
			setResponseCode(400);
			throw runtime_error("Error: invalid http head line");
		}
		if (element_headline[0] == "GET")
			return new RGet(element_headline[1], header_key_val, mServer);
		else if (element_headline[0] == "POST")
			return new RPost(element_headline[1], header_key_val, mServer);
		else if (element_headline[0] == "DELETE")
			return new RDelete(element_headline[1], header_key_val, mServer);
		else
			throw runtime_error("Error: invalid http method");
	}
	catch(const std::exception& e)
	{
		mReadStatus = ERROR;
		std::cerr << e.what() << '\n';
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
	pid_t	pid = request->operate();

	if (pid)
	{
		mKq.addProcessPid(pid, reinterpret_cast<void*>(this));
		mRequestStatus = PROCESSING;
		cout << "not SENDING" << endl;
	}
	else
	{
		mRequestStatus = SENDING;
		cout << "check SENDING\n";		
	}
}

void			Client::writeSocket(struct kevent* event)
{
	if(!mResponseMSG.size())
		mResponseMSG = mRequests.front()->createResponse();
	if (sendResponseMSG(event))
	{
		mRequests.pop();
		if(!mRequests.empty())
			operateRequest(mRequests.front());
		else
			mRequestStatus = EMPTY;
	}
}

int			Client::sendResponseMSG(struct kevent* event)
{
		size_t	sendinglen = getResponseMSG().size() - getRequests().front()->mSendLen;
		if (static_cast<size_t>(event->data) <= sendinglen)
			sendinglen = event->data;
		sendinglen = send(getFd(), getResponseMSG().c_str() + getRequests().front()->mSendLen, sendinglen, 0);
		getRequests().front()->mSendLen += sendinglen;
		if (getRequests().front()->mSendLen == getResponseMSG().size())
			return 1;
		return 0;
}

void			Client::handleProcess(struct kevent* event)
{
	if (event->data != 0)
		mRequests.front()->setCode(500);
	else
		mRequests.front()->setCode(201);
	mRequestStatus = SENDING;
}



void			Client::resetTimer(int mKq, struct kevent event)
{
	event.filter = EVFILT_TIMER;
	event.fflags = NOTE_SECONDS;
	event.data = TIMEOUT_SEC;
	event.flags = EV_ADD | EV_ONESHOT;
	if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
		throw runtime_error("Error: resetTimer Failed");
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
queue<ARequest*>	Client::getRequests() const { return mRequests; }
string&				Client::getResponseMSG() { return mResponseMSG; }
string&				Client::getHeadBuffer() { return mInputBuffer; }
string&				Client::getInputBuffer() { return mInputBuffer; }
int					Client::getResponseCode() const { return mResponseCode; }
pid_t				Client::getCGI() const { return mCGI; }
int					Client::getRequestStatus() const { return mRequestStatus; }

void				Client::setReadStatus(int mStatus) { this->mReadStatus = mStatus; }
void				Client::setResponseCode(int code) { mResponseCode = code; }
void				Client::setCGI(pid_t mCGI) { this->mCGI = mCGI; }