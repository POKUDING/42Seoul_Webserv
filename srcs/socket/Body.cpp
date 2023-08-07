#include "../../includes/socket/Body.hpp"

// constructor and destructor

Body::Body() :  mReadEnd(false), mChunked(false), mSendLen(0), mContentLen(0), mChunkLen(0) {}
Body::~Body() {}

// memeber functions

// public

int	Body::addBody(InputBuffer& inputBuffer)
{
	if (mChunked) {
		return (addChunkBody(inputBuffer));
	} else {
		return (addLenBody(inputBuffer));
	}
}

void	Body::writeBody(int fd)
{
	size_t	sendLen = 0;
	size_t	sendingLen = mBody.size() - mSendLen;

	if (sendingLen)
		sendLen = write(fd, mBody.c_str() + mSendLen, sendingLen);
	if (sendingLen && sendLen <= 0) {
		close (fd);
		cerr << "input to pipe error" << endl;
		throw 500;
	}
	mSendLen += sendLen;
	if (mBody.size() == mSendLen && mReadEnd) {
		cout << "write pipe end" << endl;
		close (fd);
	}
}

// getters and setters

size_t	Body::getSize() { return mBody.size(); }
size_t	Body::getMaxBodySize() { return mMaxBodySize; }
bool	Body::getChunked() { return mChunked; }
size_t	Body::getSendLen() { return mSendLen; }
size_t	Body::getContentLen() { return mContentLen; }
bool	Body::getReadEnd() { return mReadEnd; }
string&	Body::getBody() { return mBody; }
void	Body::setMaxBodySize(size_t mMaxbody) { this->mMaxBodySize = mMaxbody; }
void	Body::setChunked(bool chunk) { this->mChunked = chunk; }
void	Body::setSendLen(size_t mSendLen) { this->mSendLen = mSendLen; }
void	Body::setContentLen(size_t len) { this->mContentLen = len; }

// private

int	Body::addChunkBody(InputBuffer& inputBuffer)
{
	string	input_tmp;

	//body + header의 경우 error 던져짐
	if (mChunkLen == 0)
		mChunkLen = parseChunkLen(inputBuffer);
	// cout << "mChunkLen: "<<mChunkLen <<", input size : " <<inputBuffer.size() - inputBuffer.getIndex() << endl;
	while (mReadEnd == false && mChunkLen && inputBuffer.size() - inputBuffer.getIndex())
	{
		if (inputBuffer.size() - inputBuffer.getIndex() >= mChunkLen + 2) {
			mBody.append(inputBuffer.getCharPointer(), mChunkLen);
			if (inputBuffer.getCharPointer()[mChunkLen] != '\r' || inputBuffer.getCharPointer()[mChunkLen + 1] != '\n')
				throw 400;
				// throw runtime_error("Error: invalid chunk format");
			inputBuffer.updateIndex(inputBuffer.getIndex() + mChunkLen + 2);
			mChunkLen = parseChunkLen(inputBuffer);
		} else
			break;
	}
	if (mReadEnd == true && inputBuffer.size() - inputBuffer.getIndex() - mChunkLen >= 2) {
		inputBuffer.reset(inputBuffer.getIndex() + 2);
		return 1;
	}
	return 0;

}

size_t	Body::parseChunkLen(InputBuffer& inputbuff)
{
	char 	*end_ptr;
	size_t	len;

	len = strtol(inputbuff.getCharPointer(), &end_ptr, 16);
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

int	Body::addLenBody(InputBuffer& inputBuffer)
{
	//body + header의 경우 error 던져짐
	if (inputBuffer.size() - inputBuffer.getIndex() < mContentLen)
		return 0;
	mBody.append(inputBuffer.getCharPointer(), mContentLen);
	if (inputBuffer.size() - inputBuffer.getIndex() != mContentLen)
		throw 413;
	inputBuffer.reset();
	mReadEnd = true;

	return 1;
}
