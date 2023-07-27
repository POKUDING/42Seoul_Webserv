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
	if (kevent(mKq, &mChangeList[0], mChangeList.size(), NULL, 0, NULL) == -1)
		throw runtime_error("Error: kevent change failed");
	mChangeList.clear();
	return kevent(mKq, NULL, 0, mEvents, MAX_EVENT, NULL);

}

void	KQueue::setNextEvent(int RequestStatus, int fd, void* udata)
{
	struct kevent event;

	if (RequestStatus == SENDING)
		EV_SET(&event, fd, EVFILT_WRITE, EV_ENABLE | EV_CLEAR, 0, 0, udata);
	else // PROCESSING || EMPTY
		EV_SET(&event, fd, EVFILT_WRITE, EV_DISABLE | EV_CLEAR, 0, 0, udata);
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
	EV_SET(&event, fd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, udata);
	mChangeList.push_back(event);
	// if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
	// throw runtime_error("Error: addClientSocket error");
}

void	KQueue::addProcessPid(pid_t pid, void* udata)
{
	struct kevent event;
	EV_SET(&event, pid, EVFILT_PROC, EV_ADD, 0, 0, udata);
	mChangeList.push_back(event);
	// if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
	// throw runtime_error("Error: addClientSocket error");
}

void	KQueue::deleteClientSocketFd(int fd)
{
	struct kevent	event;


	close(fd); // close() -> 이벤트 자동 삭제라는 정보가 있음
	EV_SET(&event, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	mChangeList.push_back(event);
	// if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
		// throw runtime_error("Error: FAILED - deleteClientKQ - read");
	event.filter = EVFILT_TIMER;
	mChangeList.push_back(event);
	// if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
		// throw runtime_error("Error: FAILED - deleteClientKQ - timer");
	event.filter = EVFILT_WRITE;
	mChangeList.push_back(event);
	// if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
		// throw runtime_error("Error: FAILED - deleteClientKQ - write");
}

// getters

int						KQueue::getKq() { return mKq; }
const struct kevent*	KQueue::getEvents() { return mEvents; }