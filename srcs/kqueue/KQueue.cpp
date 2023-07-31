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

void KQueue::addEvents()
{
	// cout << mChangeList.size() << endl;
	if (mChangeList.size()) {
		// cout << "KQ event add => ident : " <<  mChangeList[0].ident << " filter: " << mChangeList[0].filter << " fflags " << mChangeList[0].fflags << " flags " <<mChangeList[0].flags << endl;
		kevent(mKq, &mChangeList[0], mChangeList.size(), NULL, 0, NULL);
	}
	mChangeList.clear();
}

int	KQueue::getEventNum()
{
	memset(mEvents, 0, sizeof(MAX_EVENT));
	addEvents();
	return kevent(mKq, NULL, 0, mEvents, MAX_EVENT, NULL);

}

void	KQueue::setNextEvent(int RequestStatus, int fd, void* udata)
{
	struct kevent event;

	cout << "\n============== after Client handler, request status: " << RequestStatus  << "\n" << endl; 
	if (RequestStatus == SENDING)
		EV_SET(&event, fd, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, udata);
	else // PROCESSING || EMPTY
		EV_SET(&event, fd, EVFILT_WRITE, EV_DELETE | EV_CLEAR, 0, 0, udata);
	// if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
		// throw runtime_error("Error: setNextEvent error");
	mChangeList.push_back(event);
}

void	KQueue::addServerSocketFd(int fd, void* udata) // SpiderMen::initSocket
{
	struct kevent event;

	EV_SET(&event, fd, EVFILT_READ, EV_ADD, 0, 0, udata);
	mChangeList.push_back(event);
	// if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
	// 	throw runtime_error("Error: kevent add failed");
}

void	KQueue::addClientSocketFd(int fd, void* udata)
{
	struct kevent event;

	EV_SET(&event, fd, EVFILT_READ, EV_ADD, 0, 0, udata);
	mChangeList.push_back(event);
	// if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
		// throw runtime_error("Error: addClientSocket error");
	EV_SET(&event, fd, EVFILT_TIMER, EV_ADD | EV_ONESHOT, NOTE_SECONDS, TIMEOUT_SEC, udata);
	mChangeList.push_back(event);
	// if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
		// throw runtime_error("Error: addClientSocket error");
	// EV_SET(&event, fd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, udata);
	// mChangeList.push_back(event);
	// if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
	// throw runtime_error("Error: addClientSocket error");
	addEvents();
}

void	KQueue::addProcessPid(pid_t pid, void* udata)
{
	struct kevent event;
	EV_SET(&event, pid, EVFILT_PROC, EV_ADD | EV_ONESHOT , NOTE_EXIT | NOTE_EXITSTATUS , 0, udata);
	mChangeList.push_back(event);
	// if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
	// throw runtime_error("Error: addClientSocket error");
}

void	KQueue::deleteProcesssPid(pid_t pid)
{
	struct kevent event;
	EV_SET(&event, pid, EVFILT_PROC, EV_DELETE, 0, 0, NULL);
	// mChangeList.push_back(event);
	if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
		cout << "Error: deleteClientSocket error\n";
}

void	KQueue::deleteClientSocketFd(int fd)
{
	struct kevent	event;

	EV_SET(&event, fd, EVFILT_TIMER, EV_DISABLE, 0, 0, NULL);
	// mChangeList.push_back(event);
	// if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
		// throw runtime_error("Error: FAILED - deleteClientKQ - read");
	// event.filter = EVFILT_TIMER;
	kevent(mKq, &event, 1, NULL, 0, NULL);

	// event.filter = EVFILT_WRITE;
	// mChangeList.push_back(event);
	// if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
		// throw runtime_error("Error: FAILED - deleteClientKQ - write");
	// addEvents();
}

// getters

int						KQueue::getKq() { return mKq; }
struct kevent*			KQueue::getEvents() { return mEvents; }