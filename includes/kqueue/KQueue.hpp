#pragma once
#ifndef KQueue_HPP
#define KQueue_HPP

#include <sys/event.h>
#include <unistd.h>
#include <vector>


#include "../basic.hpp"
#include "../config/Server.hpp"
#include <fcntl.h>

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
		void			addPipeFd(int writeFd, int readFd, void* udata);
		void			deleteProcessPid(int pid);
		void			deleteTimer(int fd);
		void			resetTimer(int fd, void* udata);

		int				getKq();
		struct kevent*	getEvents();

	private:
		int						mKq;
		vector<struct kevent>	mChangeList;
		struct kevent			mEvents[MAX_EVENT];
		
		void 			changeEvents();
};

#endif // KQUEUE_HPP