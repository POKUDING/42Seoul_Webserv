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

int	KQueue::getEventNum()
{
	memset(mEvents, 0, sizeof(MAX_EVENT));
	changeEvents();
	return kevent(mKq, NULL, 0, mEvents, MAX_EVENT, NULL);
}

void	KQueue::setNextEvent(int RequestStatus, int fd, void* udata)
{
	struct kevent event;

	// cout << "\n============== after Client handler, request status: " << RequestStatus  << "\n" << endl; 
	if (RequestStatus == SENDING) {
		EV_SET(&event, fd, EVFILT_WRITE, EV_ADD, 0, 0, udata);
		// mChangeList.push_back(event);
		if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
			throw runtime_error("evadd1 failed");
		EV_SET(&event, fd, EVFILT_READ, EV_DELETE, 0, 0, udata);
		// mChangeList.push_back(event);
		// if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
		// 	throw runtime_error("evadd2 failed");
		kevent(mKq, &event, 1, NULL, 0, NULL);
	}
	else {
		EV_SET(&event, fd, EVFILT_WRITE, EV_DELETE, 0, 0, udata);
		// mChangeList.push_back(event);
		// if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
		// 	throw runtime_error("evadd3 failed");
		kevent(mKq, &event, 1, NULL, 0, NULL);
		EV_SET(&event, fd, EVFILT_READ, EV_ADD, 0, 0, udata);
		// mChangeList.push_back(event);
		if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
			throw runtime_error("evadd4 failed");
	}
}

void	KQueue::addServerSocketFd(int fd, void* udata) // SpiderMen::initSocket
{
	struct kevent event;

	EV_SET(&event, fd, EVFILT_READ, EV_ADD, 0, 0, udata);
	mChangeList.push_back(event);
}

void	KQueue::addClientSocketFd(int fd, void* udata)
{
	struct kevent event;

	EV_SET(&event, fd, EVFILT_READ, EV_ADD, 0, 0, udata);
	mChangeList.push_back(event);
	EV_SET(&event, fd, EVFILT_TIMER, EV_ADD, NOTE_SECONDS, TIMEOUT_SEC, udata);
	mChangeList.push_back(event);
}

void	KQueue::addProcessPid(pid_t pid, void* udata)
{
	struct kevent event;
	EV_SET(&event, pid, EVFILT_PROC, EV_ADD | EV_ONESHOT , NOTE_EXIT | NOTE_EXITSTATUS , 0, udata);
	mChangeList.push_back(event);
}

void	KQueue::addPipeFd(int writeFd, int readFd, void* udata)
{
	struct kevent event;

	if (writeFd)
	{
		EV_SET(&event, writeFd, EVFILT_WRITE, EV_ADD, 0, 0, udata);
		mChangeList.push_back(event);
	}
	if (readFd)
	{
		EV_SET(&event, readFd, EVFILT_READ, EV_ADD, 0, 0, udata);
		mChangeList.push_back(event);
	}
}


void	KQueue::deleteTimer(int fd)
{
	struct kevent event;

	EV_SET(&event, fd, EVFILT_TIMER, EV_DISABLE, 0, 0, 0);
	// if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
	// 	throw runtime_error("deleteTimer failed");
	kevent(mKq, &event, 1, NULL, 0, NULL);
}

void	KQueue::resetTimer(int fd, void* udata)
{
	struct kevent event;

	EV_SET(&event, fd, EVFILT_TIMER, EV_ADD, NOTE_SECONDS, TIMEOUT_SEC, udata);
	// mChangeList.push_back(event);

	// if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
	// 	throw runtime_error("deleteTimer failed");
	kevent(mKq, &event, 1, NULL, 0, NULL);
}

// getters

int						KQueue::getKq() { return mKq; }
struct kevent*			KQueue::getEvents() { return mEvents; }

// private

void KQueue::changeEvents()
{
	if (mChangeList.size()) {
		kevent(mKq, &mChangeList[0], mChangeList.size(), NULL, 0, NULL);
		// if (kevent(mKq, &mChangeList[0], mChangeList.size(), NULL, 0, NULL) == -1)
			// throw runtime_error("kevent add failed");
	}
	mChangeList.clear();
}
