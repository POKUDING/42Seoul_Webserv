#pragma once
#ifndef KQueue_HPP
#define KQueue_HPP

#include <sys/event.h>
#include <vector>

#include "../basic.hpp"
#include "../config/Server.hpp"

using namespace std;

class KQueue
{
	public:
		KQueue();
		// virtual ~KQueue();

		int		getEventNum();
		void	setNextEvent(int RequestStatus, int fd, void* udata);
		void	addServerSocket(int fd, void* udata);
		void	addClientSocket(int fd, void* udata);
		void	deleteClientSocket(int fd);

	private:
		int				mKq;
		vector<struct kevent>	mChangeList;
		struct kevent	mEvents[MAX_EVENT];
};

#endif // KQUEUE_HPP