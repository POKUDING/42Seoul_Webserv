#include "../../includes/request/Body.hpp"

int	Body::addBody(char* input, size_t size)
{
	if (mChunked)
		return (addChunkBody(input, size));
	else
		return (addLenBody(input, size));
}

int	Body::addChunkBody(char* input, size_t size)
{
	char*	cur_ptr;
	string	input_tmp;

	mChunkBuf.append(input, size);
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

	len = strtod(ChunkBuf.c_str(), &end_ptr);
	if (end_ptr[0] != '/r' || end_ptr[1] != '/n')
		return 0;
	if (len == 0)
		mReadEnd = true;
	ChunkBuf = ChunkBuf.substr(ChunkBuf.find("/r/n") + 2);
	return len;
}

int	Body::addLenBody(char* input, size_t size)
{
	if (mBody.size() + size > mContentLen)
		throw runtime_error("Error: invlaid len body format");
	mBody.append(input, size);
	if (mBody.size() == mContentLen)
	{
		mReadEnd = true;
		return 1;
	}
	return 0;
}
