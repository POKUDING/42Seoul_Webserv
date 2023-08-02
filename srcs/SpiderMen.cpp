#include "../includes/SpiderMen.hpp"

// constructors and destructor

SpiderMen::SpiderMen(const Config& config) { this->initServerSockets(config.getServer()); }
SpiderMen::~SpiderMen()
{
	for (size_t i = 0, end = mServerSockets.size(); i < end; ++i)
		close(mServerSockets[i].getFd());
	for (map<int, Client>::iterator it = mClients.begin(); it != mClients.end(); ++it)
		close(it->first);
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
		for (int i = 0; i < eventNum; ++i) {
			if (mKq.getEvents()[i].ident == 0)
				break;
			// TEST_CODE : filter print
			// if (mKq.getEvents()[i].filter == EVFILT_TIMER) {
			// 	cout << "TIMER" << endl;
			// } else if (mKq.getEvents()[i].filter == EVFILT_PROC) {
			// 	cout << "proces" << endl;
			// }

			Socket* sock_ptr = reinterpret_cast<Socket *>(mKq.getEvents()[i].udata);

			// TEST_CODE : kqueue
			cout << "Event rise => socket: " << sock_ptr->getFd() << ", type: " << sock_ptr->getType() << ", filter: " << mKq.getEvents()[i].filter << endl;
			
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
					if (fd) {
						deleteClient(fd);
					}
				} catch (const exception& e) {
					//정의하지 않은 다른 에러 처리를 위해 넣어둠: 해당 server socket새로 만들어야 할 수도 있음
					cout << "Error: Server Handler: " << e.what() << endl;
				}
			} else if (sock_ptr->getType() == CLIENT) {
				try {
					//client error의 경우 close하는 것으로 우선 진행
					if (mKq.getEvents()[i].flags == EV_ERROR)
					{
						cerr << "ev_error flag" << endl;
						throw 0;
					}
					this->handleClient(&mKq.getEvents()[i], reinterpret_cast<Client *>(sock_ptr));
					// reinterpret_cast<Client *>(sock_ptr)->resetTimer(mKq.getKq(), mKq.getEvents()[i]);
				} catch (int error) {
					cout << "Error: Client Handler: "<< sock_ptr->getFd() << ", error status: " << error<< endl;
					if (error) {
						reinterpret_cast<Client *>(sock_ptr)->setResponseCode(error);
						handleError(reinterpret_cast<Client *>(sock_ptr));
					} else {
						deleteClient(sock_ptr->getFd());
						cout << "======================= END of Error: client closed" << endl;
					}
				} catch (const exception& e) {
					cout << "Error: Client Handler: " << e.what() << ", code: " << reinterpret_cast<Client *>(sock_ptr)->getResponseCode()<< endl;
					cout << "CHANGE THIS EXCEPTION to INTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT" << endl;
					cout << "======================= END of Error" << endl;
				}
			}
		}
	}
}

// private

void	SpiderMen::deleteClient(int fd)
{
	mKq.deleteTimer(fd);
	close(fd);
	mClients.erase(fd);
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

	// CONF FILE CHECK =======================================
	// cout << "mKq: " << mKq << endl;	
	// for (size_t i = 0; i < mServerSockets.size(); ++i) {
	// 	cout << "mServerSockets " << i+1 << ": " << mServerSockets[i].getPortNumber() << endl;
	// 	for (size_t j = 0; j < mServerSockets[i].getServer()->size(); ++j) {
	// 		cout << "	" << j+1 << ": " << endl;
	// 		(*(mServerSockets[i].getServer()))[j].printMembers();
	// 	}
	// }
	// END OF (CONF FILE CHECK) =======================================
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
	Client client_tmp(CLIENT, fd, sock->getPortNumber(), sock->getServer(), mKq);
//클라이언트 소켓은 논블록처리하면 안된다는 말이 있던데 확인해봐야 합니다
	// if (fcntl(client_tmp.getFd(), F_SETFL, O_NONBLOCK) == -1)			//소켓 논블록처리
	// 	throw runtime_error("Error: client socket nonblock failed.");

	mClients.insert(pair<int, Client>(fd, client_tmp));
	mKq.addClientSocketFd(fd, reinterpret_cast<void *>(&(mClients.find(fd)->second)));
}

void	SpiderMen::handleClient(struct kevent* event, Client* client)
{
	cout << "\n============== Client handler: ";
	if (event->filter == EVFILT_READ) {
		cout << "READ" << endl;
		client->handleClientRead(event);
	} else if (event->filter == EVFILT_WRITE) {
		cout << "WRITE" << endl;
		client->handleClientWrite(event);
	} else if (event->filter == EVFILT_PROC) {
		cout << "PROC" << endl;
		client->handleProcess(event);
	} else if (event->filter == EVFILT_TIMER) {
		cout << "TIMER" << endl;
		switch (client->getReadStatus())
		{
		case EMPTY:
			throw 0;
			// throw runtime_error("TIMER: empty");
			// break;

		case PROCESSING:
			throw 500;
			// client->setResponseCode(500);;
			// throw runtime_error("TIMER: Processing");

		case SENDING:
			throw 0;
			// throw runtime_error("TIMER: Sending");
			// break;

		default:
			break;
		}
	} else {
		cout << "ERROR_EVFILT" << endl;
		throw 0;
		// client->setResponseCode(0);
		// throw runtime_error("Error: undefined EVFILT");
	}
	mKq.setNextEvent(client->getRequestStatus(), client->getFd(), client);
}

void	SpiderMen::handleError(Client* client)
{
	for (size_t i = 0, end = client->getRequests().size(); i < end; ++i)
	{
		delete client->getRequests().front();
		client->getRequests().front() = NULL;
		client->getRequests().pop();
	}
	cout << "handleError called: " << client->getRequests().size() << endl;
	client->getRequests().push(new RBad(client->getResponseCode()));
	cout << "handleError called after push: " << client->getRequests().size() << endl;
	
	cout << "--> call operate request 4" << endl;
	client->operateRequest(client->getRequests().front());

	// client->setRequestStatus(SENDING);
	mKq.setNextEvent(client->getRequestStatus(), client->getFd(), client);
	
	
}