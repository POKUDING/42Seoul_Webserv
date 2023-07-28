#include "../../includes/socket/Body.hpp"

// constructor and destructor

Body::Body() :  mReadEnd(false), mChunked(false), mContentLen(0), mChunkLen(0) {}
Body::~Body() {}

// memeber functions

// public

int	Body::addBody(string& inputbuffer)
{
	if (mChunked)
		return (addChunkBody(inputbuffer));
	else
		return (addLenBody(inputbuffer));
}

int	Body::addChunkBody(string& inputbuff)
{
	string	input_tmp;

	mChunkBuf.append(inputbuff.c_str(), inputbuff.size());
	inputbuff.clear();
	if (mChunkLen == 0)
		mChunkLen = parseChunkLen(mChunkBuf);
	while (mChunkLen)
	{
		if (mChunkBuf.size() < mChunkLen)
		{
			mBody.append(mChunkBuf, mChunkBuf.size());
			mChunkLen -= mChunkBuf.size();
		}
		else
		{
			mBody.append(mChunkBuf, mChunkLen);
			mChunkBuf = mChunkBuf.substr(mChunkLen);
			if (mChunkBuf.find("/r/n") != 0)
				throw runtime_error("Error: invalid chunk format");
			mChunkBuf = mChunkBuf.substr(2);
			mChunkLen = parseChunkLen(mChunkBuf);
		}
	}
	if (mReadEnd == true)
		return 1;
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
	return len;
}

int	Body::addLenBody(string& inputbuffer)
{
	if (mBody.size() + inputbuffer.size() > mContentLen + 4)
		throw runtime_error("Error: invlaid len body format");
	mBody.append(inputbuffer.c_str(), inputbuffer.size());
	inputbuffer.clear();
	if (mBody.size() == mContentLen)
	{
		mReadEnd = true;
		return 1;
	}
	return 0;
}

// getters and setters


void	Body::setContentLen(size_t len) { this->mContentLen = len; }
void	Body::setChunked(bool chunk) { this->mChunked = chunk; }

size_t	Body::getContentLen() { return mContentLen; }
bool	Body::getReadEnd() { return mReadEnd; }
string	Body::getBody() { return mBody; }