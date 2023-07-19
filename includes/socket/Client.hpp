#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../request/ARequest.hpp"
#include "./ASocket.hpp"

class Client : public ASocket
{
public:
	Client(bool mType, int mFd, int mPort, const vector<Server>* mServer);
	virtual ~Client();

	unsigned int	getTime() const;
	int				getStatus() const;
	int				getIdx() const;

	void			setTime(unsigned int mTime);
	void			setStatus(int mStatus);
	void			setIdx(int mIdx);

private:
	unsigned int	mTime;
	int				mStatus;
	int				mIdx;
	
};

#endif //CLIENT_HPP