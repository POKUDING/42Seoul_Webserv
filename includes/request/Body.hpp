#pragma once
#ifndef BODY_HPP
#define BODY_HPP

#include <string>

using namespace std;

class Body
{
	public:
		Body(void);
		Body(const Body& src);
		virtual ~Body(void);
		Body& operator=(Body const& rhs);
		int		addBody(char *input, size_t size);
		int		addChunkBody(char *input, size_t size);
		int		addLenBody(char *input, size_t size);
		void	cutInput();
		size_t	parseChunkLen(string& ChunkBuf);
	private:
		bool	mChunked;
		bool	mReadEnd;
		size_t	mContentLen;
		size_t	mChunkLen;
		string	mChunkBuf;
		string	mBody;

};

#endif