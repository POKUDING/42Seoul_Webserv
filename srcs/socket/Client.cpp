#include "../../includes/socket/Client.hpp"

Client::Client(bool mType, int mFd, int mPort, const vector<Server>* mServer)
			: ASocket(mType, mFd, mPort, mServer), mRequest(NULL), mStatus(0) ,mResponseCode(0) { }

Client::~Client()
{
	if (mRequest)
		delete mRequest;
}


int				Client::getStatus() const { return mStatus; }
ARequest*		Client::getRequest() const { return mRequest; }
string&			Client::getResponseMSG() { return mResponseMSG; }
string&			Client::getHeadBuffer() { return mHeadBuffer; }
string&			Client::getBodyBuffer() { return mBodyBuffer; }
int				Client::getResponseCode() const { return mResponseCode; }

void			Client::setStatus(int mStatus) { this->mStatus = mStatus; }
void			Client::setRequestNull() { this->mRequest = NULL; }
void			Client::setResponseCode(int code) { mResponseCode = code; }
void			Client::addBuffer(char *input, size_t size)
{
	size_t pos = 0;

	if (mStatus == nStatus::READING_HEADER)	{
		mHeadBuffer.append(input, size);
		pos = mHeadBuffer.find("\r\n\r\n");
		if (pos != string::npos) {
			mBodyBuffer.append(&mHeadBuffer[pos + 4], mHeadBuffer.size() - (pos + 4));
			mHeadBuffer = mHeadBuffer.substr(0, pos);
			mStatus = nStatus::READING_BODY;
			//header parse start
			createRequest(mHeadBuffer);

			switch (mRequest->getType())
			{
			case nMethod::GET:
			case nMethod::DELETE:
				if (mBodyBuffer.size() > 8) {
					setResponseCode(400);
					throw runtime_error("Error: Request: GET/DELETE cannot have body");
				}
				mStatus = nStatus::PROCESSING;
				break;
			
			case nMethod::POST:
				if (mRequest->getBasics().content_length == static_cast<size_t>(CHUNKED)) {
					break ;
				} else if (mBodyBuffer.size() == mRequest->getBasics().content_length) {
					// if (mBodyBuffer.find("\r\n\r\n", mRequest->getBasics().content_length - 4) == string::npos) {
					// 	setResponseCode(400);
					// 	throw runtime_error("Error: Request: POST body > end not \\r\\n\\r\\n");
					// }
					mStatus = nStatus::PROCESSING;
				} else if (mBodyBuffer.size() > mRequest->getBasics().content_length) {
					setResponseCode(400);
					throw runtime_error("Error: Request: POST body > content-length");
				}
				break;

			default:
				break;
			}

		}
	} else if (this->mStatus == nStatus::READING_BODY) {
		// POST && chunked거나 더 받아올 content-length가 있을 경우에만 해당 블록 들어옴
		
		if (mRequest->getBasics().content_length == static_cast<size_t>(CHUNKED)) {
			// CHUNKED DATA
			// check size(parsing): size in hex
			// if size != recv size => error
			// add body only
		} else {
			this->mBodyBuffer.append(input, size);
			if (mBodyBuffer.size() > mRequest->getBasics().content_length) {
				setResponseCode(400);
				throw runtime_error("Bad request:: body size not equal content length");
			} else if (mBodyBuffer.size() == mRequest->getBasics().content_length) {
				mStatus = nStatus::PROCESSING;
			}
		}
	} else {
		setResponseCode(400);
		throw runtime_error("Error: addBuffer(): recv() while processing");
	}
}

int	Client::createRequest(const string& header)
{
	vector<string>		header_line;
	vector<string>		element_headline;
	map<string,string>	header_key_val;

	header_line = SpiderMenUtil::splitString(header, "\r\n");
	header_key_val = createHttpKeyVal(header_line);
	element_headline = SpiderMenUtil::splitString(header_line[0]);

	if (count(header_line[0].begin(), header_line[0].end(), ' ') != 2 || element_headline.size() != 3 || element_headline[2] != "HTTP/1.1") {
		setResponseCode(400);
		throw runtime_error("Error: invalid http head line");
	}
	if (element_headline[0] == "GET")
		this->mRequest = new RGet(element_headline[1], header_key_val);
	else if (element_headline[0] == "POST")
		this->mRequest = new RPost(element_headline[1], header_key_val);
	else if (element_headline[0] == "DELETE")
		this->mRequest = new RDelete(element_headline[1], header_key_val);
	else {
		setResponseCode(400);
		throw runtime_error("Error: invalid http method");
	}
	return mRequest->getType();
}

map<string,string>	Client::createHttpKeyVal(const vector<string>& header_line)
{
	map<string, string>	rtn;
	size_t				pos;
	
	for (size_t i = 1, end = header_line.size(); i < end; ++i)
	{
		pos = header_line[i].find(": ");
		if (pos == string::npos) {
			setResponseCode(400);
			throw runtime_error("Error: invalid http" + header_line[i]);
		}
		
		if (rtn[header_line[i].substr(0, pos)] == "")
			rtn[header_line[i].substr(0, pos)] = header_line[i].substr(pos + 2, header_line[i].size() - pos + 2);
		else {
			throw runtime_error("Error: invalid http duplicated key");
		}
	}
	return rtn;
}

void			Client::resetTimer(int mKq, struct kevent event)
{
	event.filter = EVFILT_TIMER;
	event.fflags = NOTE_SECONDS;
	event.data = TIMEOUT_SEC;
	event.flags = EV_ADD;
	if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
		throw runtime_error("Error: resetTimer Failed");
}

void			Client::create400Response()
{
	char 	timeStamp[TIME_SIZE];
	stringstream to_str;
	string		buffer;
	string		body;

	//요청받은 파일 크기 계산 및 BODY용 stream 처리 (default:index.htmp)
	ifstream	fin("./www/errors/40x.html");
	if (fin.fail())
		throw runtime_error("Error: 400 response msg failed");
	while (getline(fin, buffer)) {
		body.append(buffer);
		body.append("\r\n", 2);
	}
	fin.close();

	//1st line: STATUS
	mResponseMSG.append("HTTP/1.1 400 Bad Request\r\n");
	
	//HEADER============================================
	Time::stamp(timeStamp);
	mResponseMSG.append(timeStamp);		//Date: Tue, 20 Jul 2023 12:34:56 GMT\r\n
	mResponseMSG.append(SPIDER_SERVER);	//Server: SpiderMen/1.0.0\r\n
	mResponseMSG.append(CONTENT_TYPE);	//Content-Type: text/html; charset=UTF-8\r\n
	
	mResponseMSG.append("Content-Length: ");
	to_str << body.size();
	to_str >> buffer;
	mResponseMSG.append(buffer);
	mResponseMSG.append("\r\n");

	mResponseMSG.append("\r\n"); //end of head

	//BODY 추가
	mResponseMSG.append(body, sizeof(body));
}

void			Client::create500Response()
{	
	char 	timeStamp[TIME_SIZE];
	stringstream to_str;
	string		buffer;
	string		body;

	//요청받은 파일 크기 계산 및 BODY용 stream 처리 (default:index.htmp)
	ifstream	fin("./www/errors/50x.html");
	if (fin.fail())
		throw runtime_error("Error: 500 response msg failed");
	while (getline(fin, buffer)) {
		body.append(buffer);
		body.append("\r\n", 2);
	}
	fin.close();

	//1st line: STATUS
	mResponseMSG.append("HTTP/1.1 500 Internal Server Error\r\n");
	
	//HEADER============================================
	Time::stamp(timeStamp);
	mResponseMSG.append(timeStamp);		//Date: Tue, 20 Jul 2023 12:34:56 GMT\r\n
	mResponseMSG.append(SPIDER_SERVER);	//Server: SpiderMen/1.0.0\r\n
	mResponseMSG.append(CONTENT_TYPE);	//Content-Type: text/html; charset=UTF-8\r\n
	
	mResponseMSG.append("Content-Length: ");
	to_str << body.size();
	to_str >> buffer;
	mResponseMSG.append(buffer);
	mResponseMSG.append("\r\n");

	mResponseMSG.append("\r\n"); //end of head

	//BODY 추가
	mResponseMSG.append(body, sizeof(body));
}

void			Client::clearClient()
{
	delete mRequest;
	mRequest = NULL;
	mStatus = nStatus::WAITING;
	mResponseCode = 0;
	mHeadBuffer.clear();
	mBodyBuffer.clear();
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