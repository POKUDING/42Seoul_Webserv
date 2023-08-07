#pragma once
#ifndef BODY_HPP
#define BODY_HPP

#include <string>
#include <iostream>
#include <unistd.h>
#include "../socket/Body.hpp"
#include "../util/InputBuffer.hpp"


using namespace std;

class Body
{
	public:
		Body();
		virtual ~Body();

		int			addBody(InputBuffer& inputBuffer);
		int			addChunkBody(InputBuffer& inputBuffer);
		int			addLenBody(InputBuffer& inputBuffer);
		size_t		parseChunkLen(InputBuffer& inputBuffer);
		int			writeBody(int fd);

		size_t		getSize();
		size_t		getMaxBodySize();
		size_t		getContentLen();
		bool		getReadEnd();
		bool		getChunked();
		string& 	getBody();
		void		setMaxBodySize(size_t mMaxbody);
		void		setChunked(bool chunk);
		void		setContentLen(size_t len);

	private:
		bool	mReadEnd;
		bool	mChunked;
		size_t	mSendLen;
		size_t	mMaxBodySize;
		size_t	mContentLen;
		size_t	mChunkLen;
		string	mChunkBuf;
		string	mBody;
};

#endif // BODY_HPP
