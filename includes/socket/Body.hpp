#pragma once
#ifndef BODY_HPP
#define BODY_HPP

#include <string>

using namespace std;

class Body
{
	public:
		Body();
		virtual ~Body();

		int		addBody(string& inputbuffer);
		int		addChunkBody(string& inputbuffer);
		int		addLenBody(string& inputbuffer);
		size_t	parseChunkLen(string& ChunkBuf);

		bool	getReadEnd();
		string& getBody();

	private:
		bool	mReadEnd;
		bool	mChunked;
		size_t	mContentLen;
		size_t	mChunkLen;
		string	mChunkBuf;
		string	mBody;
};

#endif // BODY_HPP