#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "ARequest.hpp"

class Client
{
private:
	int	mFd;
public:
	Client();
	~Client();
	void		setFd(const int& fd);
	const int&	getFd(void);
};

#endif //CLIENT_HPP