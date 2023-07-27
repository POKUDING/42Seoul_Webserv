#pragma once
#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "../config/Server.hpp"
#include "../basic.hpp"

using namespace std;

class Socket
{
	public:
		Socket(bool mType, int mFd, int mPortNumber, const vector<Server>* mServer);
		virtual ~Socket(void);

		bool					getType() const;
		int						getFd() const;
		int						getPortNumber() const;
		const vector<Server>*	getServer() const;

		// void			setServer(const vector<Server>* server);

	protected:
		const bool				mType;
		const int				mFd;
		const int				mPortNumber;
		const vector<Server>*	mServer;
};

#endif // SOCKET_HPP
