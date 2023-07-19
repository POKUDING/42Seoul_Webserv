#pragma once
#ifndef ASOCKET_HPP
#define ASOCKET_HPP

#include "../config/Server.hpp"

class ASocket
{
	public:
		ASocket(bool mType, int mFd, int mPortNumber, const vector<Server>* mServer);
		// ASocket(void);
		// ASocket(const ASocket& src);
		virtual ~ASocket(void);
		// ASocket& operator=(ASocket const& rhs);

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

#endif // ASOCKET_HPP
