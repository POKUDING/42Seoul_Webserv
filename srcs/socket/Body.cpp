#include "../../includes/socket/Body.hpp"

// constructor and destructor

Body::Body() :  mReadEnd(false), mChunked(false), mSendLen(0), mContentLen(0), mChunkLen(0) {}
Body::~Body() {}

// memeber functions

// public

int	Body::addBody(InputBuffer& inputbuffer)
{

	if (mChunked)
	{
		// cout << "chunk Body called" << endl;
		// cout << inputbuffer << endl;
		return (addChunkBody(inputbuffer));
	}
	else
	{
		// cout << "Len Body called" << endl;
		return (addLenBody(inputbuffer));
	}
}

int	Body::addChunkBody(InputBuffer& inputbuffer)
{
	string	input_tmp;

	//body + header의 경우 error 던져짐
	// mChunkBuf.append(inputbuff.c_str() + minputBuffIdx, inputbuff.size() - minputBuffIdx);
	// inputbuff.clear();
	if (mChunkLen == 0)
		mChunkLen = parseChunkLen(inputbuffer);
		// mChunkLen = parseChunkLen(inputbuff.c_str() + minputBuffIdx);
	cout << "mChunkLen: "<<mChunkLen <<", input size : " <<inputbuffer.size() - inputbuffer.getIndex() << endl;
	while (!mReadEnd && mChunkLen && inputbuffer.size() - inputbuffer.getIndex())
	{
		if (inputbuffer.size() - inputbuffer.getIndex() >= mChunkLen + 2) {
			mBody.append(inputbuffer.getCharPointer(), mChunkLen);
			// mChunkBuf = mChunkBuf.substr(mChunkLen);
			if (inputbuffer.getCharPointer()[mChunkLen] != '\r' || inputbuffer.getCharPointer()[mChunkLen + 1] != '\n')
				throw runtime_error("Error: invalid chunk format");
			inputbuffer.updateIndex(inputbuffer.getIndex() + mChunkLen + 2);
			mChunkLen = parseChunkLen(inputbuffer);
		} else
			break;
	}
	if (mReadEnd == true) {
		// cout << "chunked finished++++" << endl;
		// cout << "++++++++++++++++++++" << endl;
		cerr << "mReadEnd == true" << endl;
		cout << inputbuffer.getCharPointer() << endl;
		if (inputbuffer.size() - inputbuffer.getIndex() - mChunkLen >= 2)
		{
			cerr << "if 문 안" << endl;
			inputbuffer.reset(inputbuffer.getIndex() + 2);
			return 1;
		}
		// if (inputbuffer.size() - inputbuffer.getIndex() - mChunkLen == 2)
		// {
		// 	inputbuffer.reset();
		// 	return 1;
		// }
	}
	return 0;

}

size_t	Body::parseChunkLen(InputBuffer& inputbuff)
{
	char 	*end_ptr;
	size_t	len;

	// cout << "in Parse chunklen input buff : \n$" <<inputbuff.getCharPointer() << "$" << endl;
	len = strtol(inputbuff.getCharPointer(), &end_ptr, 16);
	// cerr << "len: " << len << endl;
	if (end_ptr[0] != '\r' || end_ptr[1] != '\n')
		return 0;
	if (len == 0) 
	{
		if (end_ptr[2] != '\r' || end_ptr[3] != '\n')
			return 0;
		mReadEnd = true;
	}
	inputbuff.updateIndex(inputbuff.getIndex() + (end_ptr - inputbuff.getCharPointer() + 2));
	return len;
}

int	Body::addLenBody(InputBuffer& inputbuffer)
{
	//body + header의 경우 error 던져짐
	if (inputbuffer.size() < mContentLen + 4)
		return 0;
	mBody.append(inputbuffer.getCharPointer(), mContentLen);
	if (inputbuffer.compare(inputbuffer.getIndex() + mContentLen, 4, "/r/n/r/n"))
		throw runtime_error("Error: invlaid len body format");
	inputbuffer.reset(inputbuffer.getIndex() + mContentLen + 4);
	mReadEnd = true;
	return 1;
}

int	Body::writeBody(int fd)
{
	size_t	sendLen = 0;
	size_t	sendingLen = mBody.size() - mSendLen;
	
	cout << "write body called sendinglen : " << sendingLen << endl;
	if (sendingLen)
		sendLen = write(fd, mBody.c_str() + mSendLen, sendingLen);
	cout << " here" << endl;
	if (sendingLen && sendLen <= 0)
	{
		close (fd);
		cerr << "input to pipe error" << endl;
		throw 500;
	}
	cout << "SendLen success : " <<sendLen <<endl;
	mSendLen += sendLen;
	if (mBody.size() == mSendLen && mReadEnd)
	{
		cout << "close pipe" << endl;
		close (fd);
	}
	return sendLen;
}

// getters and setters

void	Body::setMaxBodySize(size_t mMaxbody) { this->mMaxBodySize = mMaxbody; }
void	Body::setContentLen(size_t len) { this->mContentLen = len; }
void	Body::setChunked(bool chunk) { this->mChunked = chunk; }


size_t	Body::getSize() { return mBody.size(); }
size_t	Body::getMaxBodySize() { return mMaxBodySize; }
bool	Body::getChunked() { return mChunked; }
size_t	Body::getContentLen() { return mContentLen; }
bool	Body::getReadEnd() { return mReadEnd; }
string&	Body::getBody() { return mBody; }
