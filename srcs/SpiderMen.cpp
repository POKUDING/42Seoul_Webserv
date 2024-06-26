#include "../includes/SpiderMen.hpp"

// constructors and destructor

SpiderMen::SpiderMen(const Config& config) { this->initServerSockets(config.getServer()); }
SpiderMen::~SpiderMen()
{
	for (size_t i = 0, end = mServerSockets.size(); i < end; ++i)
		close(mServerSockets[i].getFd());
	map<int, Client>::iterator begin = mClients.begin();
	map<int, Client>::iterator end = mClients.end();
	for (;begin != end; ++begin)
		close(begin->second.getFd());
}

// member functions

// public

void	SpiderMen::run()
{
	int eventNum;

	while (1) {
		eventNum = mKq.getEventNum();
		if (eventNum == -1)
			eventNum = MAX_EVENT;
		// cout << "\nEvent num: " <<eventNum << endl;
		// cout << "clients size: " << mClients.size() << endl;
		for (int i = 0; i < eventNum; ++i) {
			if (mKq.getEvents()[i].ident == 0)
				break;
			
			Socket* sock_ptr = reinterpret_cast<Socket *>(mKq.getEvents()[i].udata);
			// cout << "filter: " << mKq.getEvents()[i].filter<< ", IDNET: "<< mKq.getEvents()[i].ident <<endl;
			// cout << "TYPE:" <<sock_ptr->getType() << ", FD: "<<sock_ptr->getFd() << ", IDENT: "<< (&mKq.getEvents()[i])->ident << endl;


			// TEST_CODE : kqueue
			// cout << "Event rise => socket: " << sock_ptr->getFd() << ", type: " << sock_ptr->getType() << ", filter: " << mKq.getEvents()[i].filter << endl;

			if (sock_ptr->getType() == SERVER) {
				try {
					if (mKq.getEvents()[i].flags == EV_ERROR) {
						mKq.getEvents()[i].flags = EV_CLEAR;
						if (kevent(mKq.getKq(), &mKq.getEvents()[i], 1, NULL, 0, NULL) == -1)
							throw runtime_error("flag update FAILED");
					}
					this->handleServer(sock_ptr);
				} catch (int fd) {
					//client socket 에러 (accept(), kevent())
					cout << "Error: Server Handler: close client [" << fd << "]" << endl;
					if (fd)
						deleteClient(fd);
				} catch (const exception& e) {
					throw e;
				}
			} else if (sock_ptr->getType() == CLIENT) {
				try {
					//client error의 경우 close하는 것으로 우선 진행
					if (mKq.getEvents()[i].flags == EV_ERROR) {
						cerr << "ev_error flag" << endl;
						throw 0;
					}
					this->handleClient(&mKq.getEvents()[i], reinterpret_cast<Client *>(sock_ptr));
					// mKq.resetTimer(sock_ptr->getFd(), sock_ptr);
					if (sock_ptr->getFd() == static_cast<int>(mKq.getEvents()[i].ident))
						mKq.resetTimer(sock_ptr->getFd(), sock_ptr);
				} catch (int error) {
					// cout << "Error: Client Handler: "<< sock_ptr->getFd() << ", error status: " << error<< endl;
					if (error && reinterpret_cast<Client *>(sock_ptr)->getRequests().empty() == false) {
						cerr << error << endl;
						reinterpret_cast<Client *>(sock_ptr)->setResponseCode(error);
						handleError(reinterpret_cast<Client *>(sock_ptr));
					} else {
						// cout << "======================= client closed [" << sock_ptr->getFd() << "]" << endl;
						deleteClient(sock_ptr->getFd());
					}
					break;
				} catch (const exception& e) {
					throw e;
				}
			}
		}
	}
}

// private

void	SpiderMen::deleteClient(int fd)
{
	if (mClients.find(fd) != mClients.end()) {
		// cout << "deleteClient: " << fd << endl;
		close(fd);
		mKq.deleteTimer(fd);
		mClients.erase(fd);
	}
}

void	SpiderMen::initServerSockets(const map<int,vector<Server> >& servers)
{
	struct sockaddr_in	server_addr;
	int					fd;

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;					//IPv4 인터넷 프로토콜
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	//호스트 주소 자동할당옵션
	for (map<int,vector<Server> >::const_iterator it = servers.cbegin(); it != servers.cend(); ++it) {
		fd = socket(AF_INET, SOCK_STREAM, 0);
		if (fd == -1)
			throw runtime_error("Error: socket open failed.");
		//소켓 옵션 reuse로 세팅
		int optval = 1;
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
		server_addr.sin_port = htons(it->first);	//사용할 포트 (hton short로 변경한 값 저장)
		if (bind(fd, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) == -1)
			throw runtime_error("Error: socket bind failed.");
		if (listen(fd, BACKLOG) == -1)
			throw runtime_error("Error: socket listen failed.");
		if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)			//소켓 논블록처리
			throw runtime_error("Error: socket nonblock failed.");
		Socket	sock(SERVER, fd, it->first, const_cast<vector<Server>*>(&(it->second)), mKq);
		mServerSockets.push_back(sock);
		mKq.addServerSocketFd(fd, reinterpret_cast<void *>(&(*(mServerSockets.rbegin()))));
	}
}

void	SpiderMen::handleServer(Socket* sock)
{
	struct sockaddr_in	client_addr;
	int					fd;

	// 새로운 연결 요청 수락
	socklen_t client_len = sizeof(client_addr);
	fd = accept(sock->getFd(), reinterpret_cast<struct sockaddr*>(&client_addr), &client_len);
	if (fd == -1) {
		throw FAIL_FD;
	}
	//RE
	struct linger opt;
    	opt.l_onoff = 1;  // linger를 활성화
    	opt.l_linger = 0;
	setsockopt(fd, SOL_SOCKET, SO_LINGER, &opt, sizeof(opt));
	
	Client client_tmp(CLIENT, fd, sock->getPortNumber(), sock->getServer(), mKq);
	if (fcntl(client_tmp.getFd(), F_SETFL, O_NONBLOCK) == -1)
		throw fd;
	mClients.insert(pair<int, Client>(fd, client_tmp));
	mKq.addClientSocketFd(fd, reinterpret_cast<void *>(&(mClients.find(fd)->second)));
}

void	SpiderMen::handleClient(struct kevent* event, Client* client)
{
	// cout << "\n============== Client handler: ";
	if (event->filter == EVFILT_READ) {
		// cout << "READ" << endl;
		client->handleClientRead(event);
	} else if (event->filter == EVFILT_WRITE) {
		// cout << "WRITE" << endl;
		client->handleClientWrite(event);
	} else if (event->filter == EVFILT_PROC) {
		// cout << "PROC" << endl;
		client->handleProcess(event);
	} else if (event->filter == EVFILT_TIMER) {
		cerr << "TIMER" << endl;
		switch (client->getRequestStatus()) {
			case EMPTY:		throw 0;
			case PROCESSING: throw 500;
			case SENDING:	throw 0;
			default:		break;
		}
	} else {
		// cout << "ERROR_EVFILT" << endl;
		throw 0;
	}
	mKq.setNextEvent(client->getRequestStatus(), client->getFd(), client);
}

void	SpiderMen::handleError(Client* client)
{
	Server server = client->getRequests().front()->getServer();

	if (client->getPid()) {
		// cerr << "pid delete!" <<endl;
		mKq.deleteProcessPid(client->getPid());
		kill(client->getPid(), 2);
		waitpid(client->getPid(), NULL, 0);
		client->setPid(0);
	}
	if (client->getRequests().front()->getReadPipe())
		close(client->getRequests().front()->getReadPipe());
	if (client->getRequests().front()->getWritePipe())
		close (client->getRequests().front()->getWritePipe());
	for (size_t i = 0, end = client->getRequests().size(); i < end; ++i) {
		delete client->getRequests().front();
		client->getRequests().front() = NULL;
		client->getRequests().pop();
	}
	// cout << "handleError called: " << client->getRequests().size() << endl;
	client->getRequests().push(new RBad(client->getResponseCode(), server));
	// cout << "handleError called after push: " << client->getRequests().size() << endl;

	// cout << "--> call operate request 4" << endl;
	client->operateRequest(client->getRequests().front());

	mKq.setNextEvent(client->getRequestStatus(), client->getFd(), client);
}
