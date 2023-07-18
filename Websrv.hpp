#pragma once
#ifndef WEBSRV_HPP
#define WEBSRV_HPP

#include <sys/types.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include "Config.hpp"
#include "Client.hpp"

class Websrv
{
	public:
		Websrv(const Config& config);
		virtual ~Websrv();
		void	run();

	private:
		map<int,const Server&>	mFdServers;
		vector<Client>			mClients;
		int						mKq;
		
		void	initSocket(const vector<Server>& servers);
		void	initKque();
		// Websrv(void);
		// Websrv(const Websrv& src);
		// Websrv& operator=(Websrv const& rhs);
};

#endif //WEBSRV_HPP