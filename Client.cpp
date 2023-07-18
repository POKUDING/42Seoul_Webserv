#include "Client.hpp"

Client::Client(): mFd(0) {}

Client::~Client() {}


void		Client::setFd(const int& fd)
{
	mFd = fd;
}

const int&	Client::getFd(void)
{
	return mFd;
}