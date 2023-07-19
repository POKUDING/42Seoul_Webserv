#include "../../includes/socket/Client.hpp"

Client::Client(bool mType, int mFd, int mPort, const vector<Server>* mServer)
			: ASocket(mType, mFd, mPort, mServer) { }

Client::~Client() { }


unsigned int	Client::getTime() const { return mTime; }
int				Client::getStatus() const { return mStatus; }
int				Client::getIdx() const { return mIdx; }

void			Client::setTime(unsigned int mTime) { this->mTime = mTime; }
void			Client::setStatus(int mStatus) { this->mStatus = mStatus; }
void			Client::setIdx(int mIdx) { this->mIdx = mIdx; }