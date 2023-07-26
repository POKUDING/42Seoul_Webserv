#pragma once
#ifndef SPIDERMEN_HPP
#define SPIDERMEN_HPP

#include <sys/types.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include "./config/Config.hpp"
#include "./socket/Client.hpp"

using namespace std;

class SpiderMen
{
	public:
		SpiderMen(const Config& config);
		virtual ~SpiderMen();
		void	run();

	private:
		map<int,const Server&>	mFdServers;
		map<int, Client>		mClients;
		deque<Socket>			mServerSockets;
		int						mKq;

		void				deleteClientKQ(int fd);
		void				deleteClient(int fd);
		void				initSocket(const map<int,vector<Server> >& servers);
		void				handleServer(Socket* sock);
		void				handleClient(struct kevent* event, Client* client);
		void				readSocket(struct kevent* event, Client* client);
		// void				initKque(x);
		// SpiderMen(void);
		// SpiderMen(const SpiderMen& src);
		// SpiderMen& operator=(SpiderMen const& rhs);
};

#endif //SPIDERMEN_HPP
