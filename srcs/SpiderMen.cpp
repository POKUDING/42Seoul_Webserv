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
			if (mKq.getEvents()[i].filter == EV_ERROR)
				throw runtime_error("Error: event filter error"); // try-catch 필요
			Socket* sock_ptr = reinterpret_cast<Socket *>(mKq.getEvents()[i].udata);			
			// TEST_CODE : kqueue
			cout << "socket: " << sock_ptr->getFd() << ", type: " << sock_ptr->getType() << ", request: " << mKq.getEvents()[i].filter << endl;
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
						deleteClientKQ(fd);
						deleteClient(fd);
					}
				} catch (const exception& e) {
					//정의하지 않은 다른 에러 처리를 위해 넣어둠: 해당 server socket새로 만들어야 할 수도 있음
					cout << "Error: Server Handler: " << e.what() << endl;
				}
			} else if (sock_ptr->getType() == CLIENT) {
				try {
					//client error의 경우 close하는 것으로 우선 진행
					if (mKq.getEvents()[i].flags == EV_ERROR) {
						throw runtime_error("kevent flags: EV_ERROR");
					}
					this->handleClient(&mKq.getEvents()[i], reinterpret_cast<Client *>(sock_ptr));
					reinterpret_cast<Client *>(sock_ptr)->resetTimer(mKq.getKq(), mKq.getEvents()[i]);
				} catch (const exception& e) {
					cout << "Error: Client Handler: " << e.what() << ", code: " << reinterpret_cast<Client *>(sock_ptr)->getResponseCode()<< endl;
					//400 || 500 에러일 경우 처리 (throw 전에 client.setResponseCode()로 코드 설정해줘야 함)
					// if (reinterpret_cast<Client *>(sock_ptr)->getResponseCode())
						// reinterpret_cast<Client *>(sock_ptr)->createErrorResponse();
						// reinterpret_cast<Client *>(sock_ptr)->getRequest()->createResponse();
						//send
//send()여기서 어떻게 하징
					// KQ 삭제, close(fd) 중 에러 throw가 가능해 일단 추가함...
					try {
						deleteClientKQ(sock_ptr->getFd());
					} catch (const exception& e) {
						cout << "deleteKQ Error :" << e.what() << endl;
					}
//ARequest삭제(client 소멸자), 서버map에서 삭제
					deleteClient(sock_ptr->getFd());
					cout << "======================= END of Error" << endl;
				}
			}
		}
	}
}

// private

void	SpiderMen::deleteClientKQ(int fd)
{
//KQ에서 삭제 (kqueue에 EV_DELETE 추가)
	if (mClients.find(fd) != mClients.end())
		mKq.deleteClientSocketFd(fd);
}

void	SpiderMen::deleteClient(int fd)
{
	//spidermen:map에서 client삭제
	//client 객체도 삭제되고, 소멸자에서 delete ARequest가 불릴것으로 예상되나... 확인해봐야합니다
	mClients.erase(fd);
}

void	SpiderMen::initServerSockets(const map<int,vector<Server> >& servers)
{
	struct sockaddr_in	server_addr;
	struct kevent		event;
	int					fd;

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;					//IPv4 인터넷 프로토콜
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	//호스트 주소 자동할당옵션
	for (map<int,vector<Server> >::const_iterator it = servers.cbegin(); it != servers.cend(); ++it) {
		fd = socket(AF_INET, SOCK	_STREAM, 0);
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
		Socket	sock(SERVER, fd, it->first, &(it->second));
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
	struct kevent		event_tmp;
	struct sockaddr_in	client_addr;
	int					fd;

	// 새로운 연결 요청 수락
	socklen_t client_len = sizeof(client_addr);
	fd = accept(sock->getFd(), reinterpret_cast<struct sockaddr*>(&client_addr), &client_len);
	if (fd == -1) {
		throw FAIL_FD;
	}
	Client client_tmp(CLIENT, fd, sock->getPortNumber(), sock->getServer());
//클라이언트 소켓은 논블록처리하면 안된다는 말이 있던데 확인해봐야 합니다
	// if (fcntl(client_tmp.getFd(), F_SETFL, O_NONBLOCK) == -1)			//소켓 논블록처리
	// 	throw runtime_error("Error: client socket nonblock failed.");

	mClients.insert(pair<int, Client>(fd, client_tmp));
	mKq.addClientSocketFd(fd, reinterpret_cast<void *>(&(mClients.find(fd)->second)));
}

// static void		testcode()
// {
// 	//make response to send
// 	client->getResponseMSG() = client->getRequest()->createResponse();

// 	//test code (request check) ===============================
// 	{
// 		ofstream test("test_request.txt", ios::app);
// 		test << client->getHeadBuffer() << client->getBodyBuffer();
// 		test.close();
// 	}
// 	//test code (response check) ===============================
// 	{
// 		ofstream test("test_response.txt", ios::app);
// 		test << client->getResponseMSG();
// 		test.close();
// 	}
// 	//END OF test code ========================================
// }

void	SpiderMen::handleClient(struct kevent* event, Client* client)
{

	if (event->filter == EVFILT_READ) {
		client->readSocket(event);
	} else if (event->filter == EVFILT_WRITE) {
		client->writeSocket(event);
		
		// size_t	sendinglen = client->getResponseMSG().size() - client->getRequest()->mSendLen;
		// if (static_cast<size_t>(event->data) <= sendinglen)
		// 	sendinglen = event->data;
		// sendinglen = send(client->getFd(), client->getResponseMSG().c_str() + client->getRequest()->mSendLen, sendinglen, 0);
		// client->getRequest()->mSendLen += sendinglen;
		// if (client->getRequest()->mSendLen >= client->getResponseMSG().size()) {
		// 	//write done
		// 	event->flags = EV_DELETE;
		// 	if (kevent(mKq, event, 1, NULL, 0, NULL) == -1)
		// 		throw runtime_error("Error: handleClient - EVFILT update Failed - DELETE");
		// 	int connection = client->getRequest()->getBasics().connection;

		// 	//request delete, client 소멸자에서 다시 delete되지 않도록 Null로 만들어줌
		// 	client->clearClient();

		// 	cout << "EOF of send, close: " << connection << endl;
		// 	if (connection == CLOSE)
		// 		throw runtime_error("client request connection CLOSE");
		// }
	} else if (event->filter == EVFILT_TIMER) {
		//check client's status
			// connected & waiting						=> close()
			// reading header							=> 400
			// reading body								=> 400
			// client request read done / processing	=> 500
			// send()									=> close()

		// switch (client->getStatus())
		// {
		// case WAITING:
		// 	throw runtime_error("TIMER: Waiting");
		// 	break;

		// case READING_HEADER:
		// case READING_BODY:
		// 	client->setResponseCode(400);
		// 	throw runtime_error("TIMER: Reading");

		// case PROCESSING:
		// 	client->setResponseCode(500);;
		// 	throw runtime_error("TIMER: Processing");

		// case SENDING:
		// 	throw runtime_error("TIMER: Sending");
		// 	break;

		// default:
		// 	break;
		// }
	} else {
		client->setResponseCode(0);
		throw runtime_error("Error: undefined EVFILT");
	}
}

