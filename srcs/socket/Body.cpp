#include "../../includes/socket/Body.hpp"

// constructor and destructor

Body::Body() :  mReadEnd(false), mChunked(false), mContentLen(0), mChunkLen(0) {}
Body::~Body() {}

// memeber functions

// public

int	Body::addBody(string& inputbuffer)
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

int	Body::addChunkBody(string& inputbuff)
{
	string	input_tmp;

	//body + header의 경우 error 던져짐
	mChunkBuf.append(inputbuff.c_str(), inputbuff.size());
	inputbuff.clear();
	if (mChunkLen == 0)
		mChunkLen = parseChunkLen(mChunkBuf);
	while (!mReadEnd && mChunkLen && mChunkBuf.size())
	{
		// cout << "BEFORE chunke len : " << mChunkLen << " mChunkBuf size : " <<mChunkBuf.size() << endl;
		if (mChunkBuf.size() < mChunkLen) {
			mBody.append(mChunkBuf.c_str(), mChunkBuf.size());
			mChunkLen -= (mChunkBuf.size());
			mChunkBuf.clear();
			// cout << "AFTER \n$" << mBody << "$" <<endl;
			// cout << "AFTER chunke len : " << mChunkLen << " mChunkBuf size : " <<mChunkBuf.size() << endl;
		} else {
			mBody.append(mChunkBuf.c_str(), mChunkLen);
			mChunkBuf = mChunkBuf.substr(mChunkLen);
			// cout << "body size :" << mBody.size() << "\n$" << mBody  << "$" << endl;
			// cout << "\\r\\n result : " << mBody.find("\r\n") << endl;
			// cout << "in buffer : \n$" << mChunkBuf <<  "$"<<endl;
			if (mBody.find("\r\n") != mBody.size() - 2)
				throw runtime_error("Error: invalid chunk format");
			mBody = mBody.substr(0, mBody.size() - 2);
			mChunkLen = parseChunkLen(mChunkBuf);
		}
	}
	if (mReadEnd == true) {
		// cout << "chunked finished++++" << endl;
		// cout << mBody << endl;
		// cout << "++++++++++++++++++++" << endl;
		if (mChunkBuf.size() >= 2)
		{
			inputbuff.append(mChunkBuf.c_str() + 2, mChunkBuf.size() - 2);
			return 1;
		}
	}
	return 0;

}

size_t	Body::parseChunkLen(string& ChunkBuf)
{
	char 	*end_ptr;
	size_t	len;

	len = strtol(ChunkBuf.c_str(), &end_ptr, 16);
	if (end_ptr[0] != '\r' || end_ptr[1] != '\n')
		return 0;
	if (len == 0)
		mReadEnd = true;
	ChunkBuf = ChunkBuf.substr(ChunkBuf.find("\r\n") + 2);
	return len + 2;
}

int	Body::addLenBody(string& inputbuffer)
{
	//body + header의 경우 error 던져짐
	mBody.append(inputbuffer.c_str(), inputbuffer.size());
	inputbuffer.clear();
	if (mBody.size() + inputbuffer.size() > mContentLen + 4)
	{
		inputbuffer = mBody.substr(mContentLen + 4);
		mBody = mBody.substr(0, mContentLen + 4);
	}
	if (mBody.size() == mContentLen + 4)
	{
		mReadEnd = true;
		if (mBody.find("\r\n\r\n", mContentLen) == string::npos)
			throw runtime_error("Error: invlaid len body format");
		return 1;
	}
	return 0;
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