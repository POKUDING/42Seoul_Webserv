#include "../../includes/kqueue/KQueue.hpp"

// constructor

KQueue::KQueue()
{
	mKq = kqueue();
	if (mKq == -1)
		throw runtime_error("Error: kqueue create failed.");
}

// member functions

// public

int		KQueue::getEventNum()
{
	// if (kevent(mKq, &mChangeList[0], mChageList.size(), NULL, 0, NULL) == -1)
	// 	throw runtime_error("Error: kevent change failed");
	// mChangeList.clear();
	return kevent(mKq, NULL, 0, mEvents, MAX_EVENT, NULL);
}
void	KQueue::setNextEvent(int RequestStatus, int fd, void* udata)
{
	struct kevent event;

	if (RequestStatus == SENDING)
		EV_SET(&event, fd, EVFILT_WRITE, EV_ENABLE | EV_CLEAR, 0, 0, udata);
	else // PROCESSING || EMPTY
		EV_SET(&event, fd, EVFILT_WRITE, EV_DISABLE | EV_CLEAR, 0, 0, udata);
	if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
	// mChangeList.push_back(event);
		throw fd;
		// throw runtime_error("Error: setNextEvent error");
}

void	KQueue::addServerSocket(int fd, void* udata) // SpiderMen::initSocket
{
	struct kevent event;

	EV_SET(&event, fd, EVFILT_READ, EV_ADD, 0, 0, udata);
	if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
		throw runtime_error("Error: kevent add failed");
	// mChangeList.push_back(event);
}

void	KQueue::addClientSocket(int fd, void* udata)
{
	struct kevent event;

	// read이벤트 등록
	EV_SET(&event, fd, EVFILT_READ, EV_ADD, 0, 0, udata);
	if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
	// mChangeList.push_back(event);
		throw fd;
		// throw runtime_error("Error: addClientSocket error");
	// mChangeList.push_back(event);
	//timer이벤트 등록
	EV_SET(&event, fd, EVFILT_TIMER, EV_ADD, NOTE_SECONDS, TIMEOUT_SEC, udata);
	if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
	// mChangeList.push_back(event);
		throw fd;
		// throw runtime_error("Error: addClientSocket error");
	// write이벤트 등록 및 disable
	EV_SET(&event, fd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, udata);
	if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
	// mChangeList.push_back(event);
		throw fd;
	// throw runtime_error("Error: addClientSocket error");
}

void	KQueue::deleteClientSocket(int fd)
{
	struct kevent	event;

	EV_SET(&event, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
		throw runtime_error("Error: FAILED - deleteClientKQ - read");
	event.filter = EVFILT_TIMER;
	if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
		throw runtime_error("Error: FAILED - deleteClientKQ - timer");
	event.filter = EVFILT_WRITE;
	if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
		throw runtime_error("Error: FAILED - deleteClientKQ - write");
}