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

class Websrv
{
	private:
		map<int,const Server&>	fd_servers;
		
		void	initSocket(const vector<Server>& servers);
		// Websrv(void);
		// Websrv(const Websrv& src);
		// Websrv& operator=(Websrv const& rhs);
	public:
		Websrv(const Config& config);
		virtual ~Websrv(void);
};

#endif //WEBSRV_HPP