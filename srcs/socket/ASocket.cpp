#include "../../includes/socket/ASocket.hpp"

// constructors and destructor

ASocket::ASocket(bool mType, int mFd, int mPortNumber, const vector<Server>* mServer)
	: mType(mType), mFd(mFd), mPortNumber(mPortNumber), mServer(mServer) { }
ASocket::~ASocket(void) { }

// member functions

bool	ASocket::getType() const { return mType; }
int		ASocket::getFd() const { return mFd; }
int		ASocket::getPortNumber() const { return mPortNumber; }
const vector<Server>*	ASocket::getServer() const { return mServer; }

// void	ASocket::setServer(const vector<Server>* server) const { mServer = server; }