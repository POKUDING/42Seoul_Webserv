#pragma once
#ifndef BODY_HPP
#define BODY_HPP

#include <string>
#include <iostream>
#include "../socket/Body.hpp"

using namespace std;

class Body
{
	public:
		Body();
		virtual ~Body();

		int			addBody(string& inputbuffer);
		int			addChunkBody(string& inputbuffer);
		int			addLenBody(string& inputbuffer);
		size_t		parseChunkLen(string& ChunkBuf);

		void		setMaxBodySize(size_t mMaxbody);
		void		setChunked(bool chunk);
		void		setContentLen(size_t len);
		size_t		getContentLen();
		bool		getReadEnd();
		bool		getChunked();
		string& 	getBody();

	private:
		bool	mReadEnd;
		bool	mChunked;
		size_t	mMaxBodySize;
		size_t	mContentLen;
		size_t	mChunkLen;
		string	mChunkBuf;
		string	mBody;
};

#endif // BODY_HPP