#pragma once
#ifndef Socket_HPP
#define Socket_HPP

#include "../config/Server.hpp"
#include "../basic.hpp"

using namespace std;

class Socket
{
	public:
		Socket(bool mType, int mFd, int mPortNumber, const vector<Server>* mServer);
		// Socket(void);
		// Socket(const Socket& src);
		virtual ~Socket(void);
		// Socket& operator=(Socket const& rhs);

		bool					getType() const;
		int						getFd() const;
		int						getPortNumber() const;
		const vector<Server>*	getServer() const;

		// void			setServer(const vector<Server>* server);

	protected:
		const bool		mType;
		const int		mFd;
		const int		mPortNumber;
		const vector<Server>*	mServer;
};

#endif // Socket_HPP
