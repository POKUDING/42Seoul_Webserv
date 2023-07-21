#include "../../includes/socket/Client.hpp"

Client::Client(bool mType, int mFd, int mPort, const vector<Server>* mServer)
			: ASocket(mType, mFd, mPort, mServer), mBuFlag(0) { }

Client::~Client() { }


unsigned int	Client::getTime() const { return mTime; }
int				Client::getStatus() const { return mStatus; }
int				Client::getIdx() const { return mIdx; }

void			Client::setTime(const unsigned int mTime) { this->mTime = mTime; }
void			Client::setStatus(const int mStatus) { this->mStatus = mStatus; }
void			Client::setIdx(const int mIdx) { this->mIdx = mIdx; }
void			Client::setMethod(const int method) { this->mMethod = method; }

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
			//parse
		}
	} else if (this->mBuFlag == 1) {
		this->mBodyBuffer.append(input, size);
	} else {
		throw runtime_error("wrong http request");
	}
}

// void	Client::parseHeader(void)
// {
// 	stringstream	stream;
// 	string			word;
// 	char			tmp;

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
// 			//1) key:
// 			//2) ' ' -------- flush
// 			//3) value(, or ;)
// 			//3-1) value	//value가 여러개 들어올 수 있지만 우리가 필요한 애들도 여러개인지 확인 필요
// 			//4) \r
// 			//5) \n -------- flush




// 			stream.clear();		//stream flush 이거 맞나요?
// 		} else {
// 			stream << mHeadBuffer[i];
// 		}
// 	}
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