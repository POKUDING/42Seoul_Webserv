#include "../../includes/kqueue/KQueue.hpp"

// constructor and destructor

KQueue::KQueue()
{
	mKq = kqueue();
	if (mKq == -1)
		throw runtime_error("Error: kqueue create failed.");
}

KQueue::~KQueue() { close(mKq); }

// member functions

// public

int	KQueue::getEventNum() { return kevent(mKq, NULL, 0, mEvents, MAX_EVENT, NULL); }

void	KQueue::setNextEvent(int RequestStatus, int fd, void* udata)
{
	struct kevent event;

	if (RequestStatus == SENDING) {
		EV_SET(&event, fd, EVFILT_WRITE, EV_ENABLE, 0, 0, udata);
		if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
			throw runtime_error("evadd1 failed");
		EV_SET(&event, fd, EVFILT_READ, EV_DISABLE, 0, 0, udata);
		if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
			throw runtime_error("evadd2 failed");
	}
	else {
		EV_SET(&event, fd, EVFILT_WRITE, EV_DISABLE, 0, 0, udata);
		if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
			throw runtime_error("evadd3 failed");
		EV_SET(&event, fd, EVFILT_READ, EV_ENABLE, 0, 0, udata);
		if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
			throw runtime_error("evadd4 failed");
	}
}

void	KQueue::addServerSocketFd(int fd, void* udata) // SpiderMen::initSocket
{
	struct kevent event;

	EV_SET(&event, fd, EVFILT_READ, EV_ADD, 0, 0, udata);
	if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
		throw runtime_error("addServerSocketFd failed");
}

void	KQueue::addClientSocketFd(int fd, void* udata)
{
	struct kevent event;

	EV_SET(&event, fd, EVFILT_READ, EV_ADD, 0, 0, udata);
	if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1) {
		throw runtime_error("addClientSocketFd failed1");
	}
	EV_SET(&event, fd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, udata);
	if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
		throw runtime_error("addClientSocketFd failed2");
	EV_SET(&event, fd, EVFILT_TIMER, EV_ADD | EV_ENABLE, NOTE_SECONDS, TIMEOUT_SEC, udata);
	if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
		throw runtime_error("addClientSocketFd failed3");
}

void	KQueue::addProcessPid(pid_t pid, void* udata)
{
	struct kevent event;
	EV_SET(&event, pid, EVFILT_PROC, EV_ADD | EV_ONESHOT , NOTE_EXIT | NOTE_EXITSTATUS , 0, udata);
	if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
		throw runtime_error("addProcessPid failed");
}

void	KQueue::addPipeFd(int writeFd, int readFd, void* udata)
{
	struct kevent event;

	if (writeFd) {
		EV_SET(&event, writeFd, EVFILT_WRITE, EV_ADD, 0, 0, udata);
		if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
			throw runtime_error("addWritePipeFd failed");

	}
	if (readFd) {
		EV_SET(&event, readFd, EVFILT_READ, EV_ADD, 0, 0, udata);
		if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
			throw runtime_error("addReadPipeFd failed");

	}
}

void	KQueue::deleteProcessPid(int pid)
{
	struct kevent event;
	int status;

	waitpid(static_cast<pid_t>(pid), &status, WNOHANG);
	if (WIFEXITED(status)) {
		return ;
	}
	EV_SET(&event, pid, EVFILT_PROC, EV_DELETE, 0, 0, NULL);
	if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1) {
		// cout << strerror(errno) << endl;
		throw runtime_error("deleteProcessPid failed");
	}
}

void	KQueue::deleteTimer(int fd)
{
	struct kevent event;

	EV_SET(&event, fd, EVFILT_TIMER, EV_DISABLE, 0, 0, NULL);
	if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
		throw runtime_error("deleteTimer failed");
}

void	KQueue::resetTimer(int fd, void* udata)
{
	struct kevent event;
	
	EV_SET(&event, fd, EVFILT_TIMER, EV_ADD | EV_ENABLE, NOTE_SECONDS, TIMEOUT_SEC, udata);

	if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
		throw runtime_error("resetTimer failed");
}

// getters

int						KQueue::getKq() { return mKq; }
struct kevent*			KQueue::getEvents() { return mEvents; }
