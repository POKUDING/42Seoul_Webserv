#include "../../includes/socket/Client.hpp"

Client::Client(bool mType, int mFd, int mPort, const vector<Server>* mServer)
			: ASocket(mType, mFd, mPort, mServer), mBuFlag(0) { }

Client::~Client() { }


unsigned int	Client::getTime() const { return mTime; }
int				Client::getStatus() const { return mStatus; }
int				Client::getIdx() const { return mIdx; }
int				Client::getBuFlag() const { return mBuFlag; }
ARequest*		Client::getRequest() const { return mRequest; }

void			Client::setTime(const unsigned int mTime) { this->mTime = mTime; }
void			Client::setStatus(const int mStatus) { this->mStatus = mStatus; }
void			Client::setIdx(const int mIdx) { this->mIdx = mIdx; }
void			Client::setBuFlag(int mBuFlag) { this->mBuFlag = mBuFlag; }

void			Client::addBuffer(char *input, size_t size)
{
	size_t pos = 0;
	if (this->mBuFlag == 0)	{
		this->mHeadBuffer.append(input, size);
		pos = mHeadBuffer.find("\r\n\r\n");
		if (pos != string::npos)
		{
			mBodyBuffer.append(&mHeadBuffer[pos + 4], mHeadBuffer.size() - pos + 4);
			mHeadBuffer = mHeadBuffer.substr(0, pos);
			this->mBuFlag = 1;
			//parse start
			if (createRequest(mHeadBuffer) != nMethod::POST)
				this->mBuFlag = 2;
		}
	} else if (this->mBuFlag == 1) {
		//recv body
		// content-length == 실제 body : OK
		// content-length < 실제 body : Bad Request or 무시 (or content-length까지만 읽기)
		// content-length > 실제 body : 무조건 FAIL

		// if !content-length, 실제 body 있을 경우: Bad Request

		
		//1) content-length
		// check cur+recv length vs content-length
		// if not okay => error

		//2) chunked data 
		// check size(parsing)
		// if size != recv size => error
		// add body only

		this->mBodyBuffer.append(input, size);

		this->mBuFlag = 2;
		
		//check if body readDone (content-length or EOF)
		// if (done) {
		// 	this->mBuFlag = 2;
		// }

	} else {
		throw runtime_error("wrong http request");
	}
}

void	Client::parseHeader(void)
{
	stringstream	stream;
	string			word;
	// char			tmp;

	// for (size_t i = 0, end = mHeadBuffer.size(); i < end; ++i) {
			//1st line
			//1) method
			//2) ' ' -------- flush
			//3) root
			//4) ' ' -------- flush
			//5) http version
			//6) \r
			//7) \n -------- flush

	// }
	for (size_t i = 0, end = mHeadBuffer.size(); i < end; ++i) {
		if (mHeadBuffer[i] == ' ' || mHeadBuffer[i] == '\n') {
			//stream check, then flush stream
			stream >> word;


			//2nd line(header) start: 순서는 상관없이 들어오는지 확인 필요(아마 상관 없을듯)

			//while (! /r/n)
			// “: “ find
			//	1) npos => error
			//	2) sth && value값 있는 지 확인 (길이)
				//		2-0) key 중복 체크
				// 		2-1) ok => save
				// 		2-2) not okay => error



			stream.clear();		//stream flush 이거 맞나요?
		} else {
			stream << mHeadBuffer[i];
		}
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

	if (count(header_line[0].begin(), header_line[0].end(), ' ') != 2 || element_headline.size() != 3 || element_headline[2] != "HTTP/1.1")
		throw runtime_error("Error: invalid http head line");
	if (element_headline[0] == "GET") 
		this->mRequest = new RGet(element_headline[1], header_key_val);
	else if (element_headline[0] == "POST")
		this->mRequest = new RPost(element_headline[1], header_key_val);
	else if (element_headline[0] == "DELETE")
		this->mRequest = new RDelete(element_headline[1], header_key_val);
	else
		throw runtime_error("Error: invalid http method");
	return mRequest->getType();
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
		
		// rtn.insert(header_line[i].substr(0, pos),header_line[i].substr(pos + 2, header_line[i].size() - pos + 2));
		// => 수정) map key중복 체크
		if (rtn[header_line[i].substr(0, pos)] == "")
			rtn[header_line[i].substr(0, pos)] = header_line[i].substr(pos + 2, header_line[i].size() - pos + 2);
		else
			throw runtime_error("Error: invalid http duplicated key");
	}
	return rtn;
}

// int	Client::checkRequest(const string& headline)
// {
// }


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