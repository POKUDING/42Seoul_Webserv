#pragma once
#ifndef SPIDERMEN_HPP
#define SPIDERMEN_HPP

#include <sys/types.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include "./config/Config.hpp"
#include "./kqueue/KQueue.hpp"
#include "./socket/Client.hpp"

using namespace std;

class SpiderMen
{
	public:
		SpiderMen(const Config& config);
		virtual ~SpiderMen();
		void	run();

	private:
		map<int, Client>	mClients;
		deque<Socket>		mServerSockets;
		KQueue				mKq;

		void				deleteClient(int fd);
		void				initServerSockets(const map<int,vector<Server> >& servers);

		void				handleServer(Socket* sock);
		void				handleClient(struct kevent* event, Client* client);
		void				handleError(Client* client);
};

#endif //SPIDERMEN_HPP
