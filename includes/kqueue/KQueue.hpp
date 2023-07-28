#pragma once
#ifndef KQueue_HPP
#define KQueue_HPP

#include <sys/event.h>
#include <unistd.h>
#include <vector>


#include "../basic.hpp"
#include "../config/Server.hpp"

using namespace std;

class KQueue
{
	public:
		KQueue();
		virtual ~KQueue();

		int				getEventNum();
		void			setNextEvent(int RequestStatus, int fd, void* udata);
		void			addServerSocketFd(int fd, void* udata);
		void			addClientSocketFd(int fd, void* udata);
		void			addProcessPid(pid_t pid, void* udata);
		// void			deleteClientSocketFd(int fd);

		int				getKq();
		struct kevent*	getEvents();

	private:
		int						mKq;
		vector<struct kevent>	mChangeList;
		struct kevent			mEvents[MAX_EVENT];
};

#endif // KQUEUE_HPP