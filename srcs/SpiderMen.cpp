#include "../includes/SpiderMen.hpp"

// constructors and destructor

SpiderMen::SpiderMen(const Config& config) : mKq(0) { this->initSocket(config.getServer()); }
SpiderMen::~SpiderMen()
{
	for (size_t i = 0, end = mServerSockets.size(); i < end; ++i)
		close(mServerSockets[i].getFd());
	for (map<int, Client>::iterator it = mClients.begin(); it != mClients.end(); ++it)
		close(it->first);
	if (mKq)
		close(mKq);
}

// getters
deque<Socket>& SpiderMen::getServerSockets() { return mServerSockets; }
map<int, Client>&	SpiderMen::getClients() { return mClients; }

// member functions
void	SpiderMen::addServerSockets(Socket& sock) { mServerSockets.push_back(sock); }
void	SpiderMen::addClients(int fd, Client& client) { mClients.insert(pair<int, Client>(fd, client)); }
void	SpiderMen::deleteClientKQ(int fd)
{
//KQ에서 삭제 (kqueue에 EV_DELETE 추가)
	struct kevent	event;
	if (mClients.find(fd) != mClients.end()) {
        EV_SET(&event, fd, EVFILT_READ, EV_DELETE, 0, 0, 0);
		if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
			throw runtime_error("Error: FAILED - deleteClientKQ - read");
		event.filter = EVFILT_TIMER;
		if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
			throw runtime_error("Error: FAILED - deleteClientKQ - timer");
		// if (mClients[fd].getStatus() == SENDING) {
		event.filter = EVFILT_WRITE;
		kevent(mKq, &event, 1, NULL, 0, NULL);
			// if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
			// 	throw runtime_error("Error: FAILED - deleteClientKQ - write");
		// }
	}
}

void	SpiderMen::deleteClient(int fd)
{
	close(fd);
	//spidermen:map에서 client삭제
	//client 객체도 삭제되고, 소멸자에서 delete ARequest가 불릴것으로 예상되나... 확인해봐야합니다
	mClients.erase(fd);
}

void	SpiderMen::initSocket(const map<int,vector<Server> >& servers)
{
	int			fd_tmp;
	struct sockaddr_in server_addr;
	struct kevent	event;

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;					//IPv4 인터넷 프로토콜
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	//호스트 주소 자동할당옵션
	mKq = kqueue();
	if (mKq == -1)
		throw runtime_error("Error: kqueue create failed.");
	for (map<int,vector<Server> >::const_iterator it = servers.cbegin(); it != servers.cend(); ++it)
	{
		fd_tmp = socket(AF_INET, SOCK_STREAM, 0);
		if (fd_tmp == -1)
			throw runtime_error("Error: socket open failed.");
		
		//소켓 옵션 reuse로 세팅
		int optval = 1;
		setsockopt(fd_tmp, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

		server_addr.sin_port = htons(it->first);	//사용할 포트 (hton short로 변경한 값 저장)
		if (bind(fd_tmp, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) == -1)
			throw runtime_error("Error: socket bind failed.");
		if (listen(fd_tmp, BACKLOG) == -1)
			throw runtime_error("Error: socket listen failed.");
		if (fcntl(fd_tmp, F_SETFL, O_NONBLOCK) == -1)			//소켓 논블록처리
			throw runtime_error("Error: socket nonblock failed.");

		Socket	sock(SERVER, fd_tmp, it->first, &(it->second));
		addServerSockets(sock);

		EV_SET(&event, fd_tmp, EVFILT_READ, EV_ADD, 0, 0, reinterpret_cast<void *>(&(*(getServerSockets().rbegin()))));
	    if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1) {
			// cout << strerror(errno) << endl;
			throw runtime_error("Error: kevent add failed.");
		}
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

void	SpiderMen::run()
{
	struct kevent events[MAX_EVENT];
	int event_num;

	while (1) {
		memset(events, 0, sizeof(events));
		event_num = kevent(mKq, NULL, 0, events, MAX_EVENT, NULL); //이벤트 듣기
		if (event_num == -1) {
			//에러 발생하면 해당 건만 error log 등으로 별도 처리,
			//웹서버는 그대로 진행되어야 함 (혹은 재시작?)

			event_num = MAX_EVENT;

			// 하기 throw 대신 error log로 저장하는 걸로 하면 될거 같아요
			// throw runtime_error("Error: Kqueue event detection failed");
		}

		//events에 있는 socket으로 type, fd 확인가능해짐
		for (int i = 0; i < event_num; ++i) {
			if (events[i].ident == 0)
				break;
//filter가 아니라 flags였습니다
//server, client 별로 대응이 달라야해서 소켓 타입 확인 후 각각의 핸들러 부르기 전에 확인하는 것으로 변경하는게 어떨까요
			if (events[i].filter == EV_ERROR) {
				//이 if 문은 하기 try문 안으로 들어가야 하지 않을까요?
				//server error => 살려둬야 함
				//client error => client 죽여 => Error Response send() 500 error

				//웹서버는 그대로 진행되어야 함 (혹은 재시작?), throw 안됨!
				throw runtime_error("Error: event filter error");
			}
			Socket* sock_ptr = reinterpret_cast<Socket *>(events[i].udata);
				
			// TEST_CODE : kqueue
			cout << "socket: " << sock_ptr->getFd() << ", type: " << sock_ptr->getType() << ", request: " << events[i].filter << endl;
	
			if (sock_ptr->getType() == SERVER) {
				try {
					if (events[i].flags == EV_ERROR) {
						events[i].flags = EV_CLEAR;
						if (kevent(mKq, &events[i], 1, NULL, 0, NULL) == -1)
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
					if (events[i].flags == EV_ERROR) {
						throw runtime_error("kevent flags: EV_ERROR");
					}
					this->handleClient(&events[i], reinterpret_cast<Client *>(sock_ptr));
					reinterpret_cast<Client *>(sock_ptr)->resetTimer(mKq, events[i]);
				} catch (const exception& e) {
					cout << "Error: Client Handler: " << e.what() << ", code: " << reinterpret_cast<Client *>(sock_ptr)->getResponseCode()<< endl;
					//400 || 500 에러일 경우 처리 (throw 전에 client.setResponseCode()로 코드 설정해줘야 함)
					if (reinterpret_cast<Client *>(sock_ptr)->getResponseCode())
						reinterpret_cast<Client *>(sock_ptr)->createErrorResponse();
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

void	SpiderMen::handleServer(Socket* sock)
{
	struct kevent		event_tmp;
	struct sockaddr_in	client_addr;
	int					fd_tmp;

	// 새로운 연결 요청 수락
	socklen_t client_len = sizeof(client_addr);
	fd_tmp = accept(sock->getFd(), reinterpret_cast<struct sockaddr*>(&client_addr), &client_len);
	Client client_tmp(CLIENT, fd_tmp, sock->getPortNumber(), sock->getServer());
	if (client_tmp.getFd() == -1) {
// fd 안만들어져서 close할거 없음 => 0
		throw FAIL_FD;
		// throw runtime_error("Error: client accept failed");
	}
//클라이언트 소켓은 논블록처리하면 안된다는 말이 있던데 확인해봐야 합니다
	// if (fcntl(client_tmp.getFd(), F_SETFL, O_NONBLOCK) == -1)			//소켓 논블록처리
	// 	throw runtime_error("Error: client socket nonblock failed.");

	// 클라이언트 server<map>에 등록
	addClients(fd_tmp, client_tmp);
	// 새로운 클라이언트 소켓을 kqueue에 등록
	EV_SET(&event_tmp, client_tmp.getFd(), EVFILT_READ, EV_ADD, 0, 0, reinterpret_cast<void *>(&(getClients().find(fd_tmp)->second)));
	if (kevent(mKq, &event_tmp, 1, NULL, 0, NULL) == -1) {
		throw fd_tmp;
		// throw runtime_error("Error: client kevent add error");
	}
//write이벤트 등록: 여기서 해도 되는지 확인 필요
	// event_tmp.filter = EVFILT_WRITE;
	// if (kevent(mKq, &event_tmp, 1, NULL, 0, NULL) == -1) {
	// 	throw FAIL_ADD_EVENT;
	// 	// throw runtime_error("Error: client kevent add error");
	// }
//timer이벤트 등록
	event_tmp.filter = EVFILT_TIMER;
	event_tmp.fflags = NOTE_SECONDS;
	event_tmp.data = TIMEOUT_SEC;
	if (kevent(mKq, &event_tmp, 1, NULL, 0, NULL) == -1) {
		throw fd_tmp;
		// throw runtime_error("Error: client kevent add error");
	}
}

void	SpiderMen::handleClient(struct kevent* event, Client* client)
{

	if (event->filter == EVFILT_READ) {
		
		if (client->getStatus() == WAITING) {
			client->setStatus(READING_HEADER);
		} else if (client->getStatus() == PROCESSING) {
			client->setResponseCode(400);
			throw runtime_error("Error: recv() while PROCESS");
		} else if(client->getStatus() == SENDING) {
			client->setResponseCode(0);
			throw runtime_error("Error: recv() while send()");
		}
		this->readSocket(event, client);
		//if (client request read done)
		if (client->getStatus() == PROCESSING) {
			
			//요청에 따라 cgi 필요해ㅐㅐㅐㅐㅐㅐㅐㅐㅐㅐㅐㅐㅐㅐㅐㅐㅐㅐㅐㅐ
			client->setCGI(fork());
			if (client->getCGI() == 0) {
				//childProcess
			}
			//getRequest().response 구조체 채워줌
			
			//make response to send
			client->getResponseMSG() = client->getRequest()->createResponse();

			//test code (request check) ===============================
			{
				ofstream test("test_request.txt", ios::app);
				test << client->getHeadBuffer() << client->getBodyBuffer();
				test.close();
			}
			//test code (response check) ===============================
			{
				ofstream test("test_response.txt", ios::app);
				test << client->getResponseMSG();
				test.close();
			}
			//END OF test code ========================================


			//event write queue add
			// setEvent(event, mState)??? 모듈화??? 굳이??? 가독성???
			event->filter = EVFILT_WRITE;
			if (kevent(mKq, event, 1, NULL, 0, NULL) == -1)
				throw runtime_error("Error: handleClient - EVFILT update Failed - write");
			client->setStatus(SENDING);
		}
	} else if (event->filter == EVFILT_WRITE) {
		size_t	sendinglen = client->getResponseMSG().size() - client->getRequest()->mSendLen;
		if (static_cast<size_t>(event->data) <= sendinglen)
			sendinglen = event->data;
		sendinglen = send(client->getFd(), client->getResponseMSG().c_str() + client->getRequest()->mSendLen, sendinglen, 0);
		client->getRequest()->mSendLen += sendinglen;
		if (client->getRequest()->mSendLen >= client->getResponseMSG().size()) {
			//write done
			event->flags = EV_DELETE;
			if (kevent(mKq, event, 1, NULL, 0, NULL) == -1)
				throw runtime_error("Error: handleClient - EVFILT update Failed - DELETE");
			event->filter = EVFILT_READ;
			event->flags = EV_ADD;
			if (kevent(mKq, event, 1, NULL, 0, NULL) == -1)
				throw runtime_error("Error: handleClient - EVFILT update Failed - DELETE");
			client->setStatus(WAITING);

			int connection = client->getRequest()->getBasics().connection;

			//request delete, client 소멸자에서 다시 delete되지 않도록 Null로 만들어줌
			client->clearClient();

			cout << "EOF of send, close: " << connection << endl;
			if (connection == CLOSE)
				throw runtime_error("client request connection CLOSE");
		}
	} else if (event->filter == EVFILT_TIMER) {
		//check client's status
			// connected & waiting						=> close()
			// reading header							=> 400
			// reading body								=> 400
			// client request read done / processing	=> 500
			// send()									=> close()

		switch (client->getStatus())
		{
		case WAITING:
			throw runtime_error("TIMER: Waiting");
			break;

		case READING_HEADER:
		case READING_BODY:
			client->setResponseCode(400);
			throw runtime_error("TIMER: Reading");

		case PROCESSING:
			client->setResponseCode(500);;
			throw runtime_error("TIMER: Processing");

		case SENDING:
			throw runtime_error("TIMER: Sending");
			break;

		default:
			break;
		}
	} else {
		client->setResponseCode(0);
		throw runtime_error("Error: undefined EVFILT");
	}
}

//readSocket은 Client에 있는게 더 맞는 거 같아요 (옮겨도 파라미터 이슈 없는지 확인은 안해봄)
void	SpiderMen::readSocket(struct kevent* event, Client* client)
{
	size_t	buffer_size = event->data;	
	char	buffer[buffer_size];
	memset(buffer, 0, buffer_size);
	ssize_t s = recv(client->getFd(), buffer, buffer_size, 0);

	if (s < 1) {
		if (s == 0) {
			throw runtime_error("client socket closed");
			// cout << "Client socket [" << client->getFd() << "] closed" << endl;
		} else {
			throw runtime_error("Error: client socket recv failed");
			// cout << "Error: Client socket [" << client->getFd() << "] recv failed" << endl;
		}
	}
	client->addBuffer(buffer, s);
}