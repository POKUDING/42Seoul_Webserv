#include "../../includes/socket/Socket.hpp"
// constructors and destructor

Socket::Socket(bool mType, int mFd, int mPortNumber, vector<Server>* mServer, KQueue& mKq)
	: mType(mType), mFd(mFd), mPortNumber(mPortNumber), mServer(mServer), mKq(mKq) { }
Socket::~Socket(void) { }

// member functions

// getters

bool					Socket::getType() const { return mType; }
int						Socket::getFd() const { return mFd; }
int						Socket::getPortNumber() const { return mPortNumber; }
vector<Server>*			Socket::getServer() const { return mServer; }

// void	Socket::setServer(const vector<Server>* server) const { mServer = server; }