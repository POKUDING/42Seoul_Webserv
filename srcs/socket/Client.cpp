#include "../../includes/socket/Client.hpp"

Client::Client(bool mType, int mFd, int mPort, const vector<Server>* mServer)
			: ASocket(mType, mFd, mPort, mServer), mRequest(NULL), mStatus(0) { }

Client::~Client()
{
	if (mRequest)
		delete mRequest;
}


int				Client::getStatus() const { return mStatus; }
int				Client::getIdx() const { return mIdx; }
// int				Client::getBuFlag() const { return mBuFlag; }
ARequest*		Client::getRequest() const { return mRequest; }

void			Client::setStatus(int mStatus) { this->mStatus = mStatus; }
void			Client::setIdx(const int mIdx) { this->mIdx = mIdx; }
// void			Client::setBuFlag(int mBuFlag) { this->mBuFlag = mBuFlag; }
void			Client::setRequestNull() { this->mRequest = NULL; }
void			Client::addBuffer(char *input, size_t size)
{
	size_t pos = 0;
	if (mStatus == nStatus::READING_HEADER)	{
		mHeadBuffer.append(input, size);
		pos = mHeadBuffer.find("\r\n\r\n");
		if (pos != string::npos) {
			mBodyBuffer.append(&mHeadBuffer[pos + 4], mHeadBuffer.size() - pos + 4);
			mHeadBuffer = mHeadBuffer.substr(0, pos);
			mStatus = nStatus::READING_BODY;

			//header parse start
			createRequest(mHeadBuffer);
// GET / DELETE 는 body를 받지 않음 => body 있으면: 에러처리
							//  => 없으면:       process 시작
// POST는 body길이 체크 => content-length == body: process 시작
				//   => content-length < body: 에러처리
				//   => content-length > body: recv() 계속 진행
				//	 => chunked data: 계속 진행
			switch (mRequest->getType())
			{
			case nMethod::GET:
			case nMethod::DELETE:
				if (mBodyBuffer.size()) {
					getRequest()->setResponse(400, "", 0);
					throw runtime_error("Error: Request: GET/DELETE cannot have body");
				}
				mStatus = nStatus::PROCESSING;
				break;
			
			case nMethod::POST:
				if (mRequest->getBasics().content_length == static_cast<size_t>(CHUNKED)) {
					break ;
				} else if (mBodyBuffer.size() == mRequest->getBasics().content_length + 8) {
					mStatus = nStatus::PROCESSING;
				} else if (mBodyBuffer.size() > mRequest->getBasics().content_length + 8) {
					getRequest()->setResponse(400, "", 0);
					throw runtime_error("Error: Request: POST body > content-length");
				}
				break;

			default:
				break;
			}

//기존 코드 지킴이
			// if (getRequest()->getType() == nMethod::POST && \
			// 	mBodyBuffer.size() == mRequest->getBasics().content_length)
			// 	this->mStatus = nStatus::PROCESSING;
			// if (mRequest->getType() != nMethod::POST)
			// 	this->mStatus = nStatus::PROCESSING;
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
			if (mBodyBuffer.size() > mRequest->getBasics().content_length + 8) {
				getRequest()->setResponse(400, "", 0);
				throw runtime_error("Bad request:: body size not equal content length");
			} else if (mBodyBuffer.size() == mRequest->getBasics().content_length + 8) {
				mStatus = nStatus::PROCESSING;
			}
		}
	} else {
		getRequest()->setResponse(400, "", 0);
		throw runtime_error("Error: addBuffer(): recv() while processing");
	}
}

// void	Client::parseHeader(void)
// {
// 	stringstream	stream;
// 	string			word;
// 	// char			tmp;

// 	// for (size_t i = 0, end = mHeadBuffer.size(); i < end; ++i) {
// 			//1st line
// 			//1) method
// 			//2) ' ' -------- flush
// 			//3) root
// 			//4) ' ' -------- flush
// 			//5) http version
// 			//6) \r
// 			//7) \n -------- flush

// 	// }
// 	for (size_t i = 0, end = mHeadBuffer.size(); i < end; ++i) {
// 		if (mHeadBuffer[i] == ' ' || mHeadBuffer[i] == '\n') {
// 			//stream check, then flush stream
// 			stream >> word;


// 			//2nd line(header) start: 순서는 상관없이 들어오는지 확인 필요(아마 상관 없을듯)

// 			//while (! /r/n)
// 			// “: “ find
// 			//	1) npos => error
// 			//	2) sth && value값 있는 지 확인 (길이)
// 				//		2-0) key 중복 체크
// 				// 		2-1) ok => save
// 				// 		2-2) not okay => error


// 			stream.clear();		//stream flush 이거 맞나요?
// 		} else {
// 			stream << mHeadBuffer[i];
// 		}
// 	}
// }

int	Client::createRequest(const string& header)
{
	vector<string>		header_line;
	vector<string>		element_headline;
	map<string,string>	header_key_val;

	header_line = SpiderMenUtil::splitString(header, "\r\n");
	header_key_val = createHttpKeyVal(header_line);
	element_headline = SpiderMenUtil::splitString(header_line[0]);

	if (count(header_line[0].begin(), header_line[0].end(), ' ') != 2 || element_headline.size() != 3 || element_headline[2] != "HTTP/1.1") {
		getRequest()->setResponse(400, "", 0);
		throw runtime_error("Error: invalid http head line");
	}
	if (element_headline[0] == "GET") 
		this->mRequest = new RGet(element_headline[1], header_key_val);
	else if (element_headline[0] == "POST")
		this->mRequest = new RPost(element_headline[1], header_key_val);
	else if (element_headline[0] == "DELETE")
		this->mRequest = new RDelete(element_headline[1], header_key_val);
	else {
		getRequest()->setResponse(400, "", 0);
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
			getRequest()->setResponse(400, "", 0);
			throw runtime_error("Error: invalid http" + header_line[i]);
		}
		
		// rtn.insert(header_line[i].substr(0, pos),header_line[i].substr(pos + 2, header_line[i].size() - pos + 2));
		// => 수정) map key중복 체크
		if (rtn[header_line[i].substr(0, pos)] == "")
			rtn[header_line[i].substr(0, pos)] = header_line[i].substr(pos + 2, header_line[i].size() - pos + 2);
		else {
			getRequest()->setResponse(400, "", 0);
			throw runtime_error("Error: invalid http duplicated key");
		}
	}
	return rtn;
}

void			Client::resetTimer(int mKq, struct kevent& event)
{
	event.filter = EVFILT_TIMER;
	event.fflags = NOTE_SECONDS;
	event.data = TIMEOUT_SEC;
	event.flags = EV_ADD;
	if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
		throw runtime_error("Error: resetTimer Failed");
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