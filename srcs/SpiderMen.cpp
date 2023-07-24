#include "../includes/SpiderMen.hpp"

// constructors and destructor

SpiderMen::SpiderMen(const Config& config) : mKq(0)
{
	this->initSocket(config.getServer());
}

SpiderMen::~SpiderMen()
{
	for (size_t i = 0, end = mServerSockets.size(); i < end; ++i)
		close(mServerSockets[i].getFd());
	for (map<int, Client>::iterator it = mClients.begin(); it != mClients.end(); ++it)
		close(it->first);
	if (mKq)
		close(mKq);
}

// getter
deque<ASocket>& SpiderMen::getServerSockets() { return mServerSockets; }
map<int, Client>&	SpiderMen::getClients() { return mClients; }

// member functions
void	SpiderMen::addServerSockets(ASocket& sock) { mServerSockets.push_back(sock); }
void	SpiderMen::addClients(int fd, Client& client) { mClients.insert(pair<int, Client>(fd, client)); }
void	SpiderMen::deleteClient(int fd) { mClients.erase(fd); }


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

		ASocket	sock(nSocket::SERVER, fd_tmp, it->first, &(it->second));
		addServerSockets(sock);

		EV_SET(&event, fd_tmp, EVFILT_READ, EV_ADD, 0, 0, reinterpret_cast<void *>(&(*(getServerSockets().rbegin()))));
	    if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1) {
			cout << strerror(errno) << endl;
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
			if (events[i].filter == EV_ERROR) {
				//이 if 문은 하기 try문 안으로 들어가야 하지 않을까요?
				//server error => 살려둬야 함
				//client error => client 죽여

				//웹서버는 그대로 진행되어야 함 (혹은 재시작?), throw 안됨!
				throw runtime_error("Error: event filter error");
			}
			
			ASocket* sock_ptr = reinterpret_cast<ASocket *>(events[i].udata);
			try {
				cout << "socket type: " << sock_ptr->getType() << " request: " << events[i].filter << endl;
				if (sock_ptr->getType() == nSocket::SERVER) {
					this->handleServer(sock_ptr);
				} else if (sock_ptr->getType() == nSocket::CLIENT) {
					this->handleClient(&events[i], reinterpret_cast<Client *>(sock_ptr));
				}
			} catch (const exception& e) {
				cout << "error in HANDLER: " << e.what() << endl;
				// exit(EXIT_FAILURE);
			}
		}
	}
	 	
}

void	SpiderMen::handleServer(ASocket* sock)
{
	struct kevent		event_tmp;
	struct sockaddr_in	client_addr;
	int					fd_tmp;

	// 새로운 연결 요청 수락
	socklen_t client_len = sizeof(client_addr);
	fd_tmp = accept(sock->getFd(), reinterpret_cast<struct sockaddr*>(&client_addr), &client_len);
	
	Client client_tmp(nSocket::CLIENT, fd_tmp, sock->getPortNumber(), sock->getServer());
	if (client_tmp.getFd() == -1)
		throw runtime_error("Error: client accept failed");
	if (fcntl(client_tmp.getFd(), F_SETFL, O_NONBLOCK) == -1)			//소켓 논블록처리
		throw runtime_error("Error: client socket nonblock failed.");

	// 새로운 클라이언트 소켓을 kqueue에 등록
	addClients(fd_tmp, client_tmp);
	EV_SET(&event_tmp, client_tmp.getFd(), EVFILT_READ, EV_ADD, 0, 0, reinterpret_cast<void *>(&(getClients().find(fd_tmp)->second)));
	if (kevent(mKq, &event_tmp, 1, NULL, 0, NULL) == -1)
		throw runtime_error("Error: client kevent add error");
}

void	SpiderMen::handleClient(struct kevent* event, Client* client)
{
	string response;

	if (event->filter == EVFILT_READ) {
		this->readSocket(event, client);
		if (client->getBuFlag() == 2) {	//if (client request read done)

			//test code (request check) ===============================
			{
				ofstream test("test_request.txt", ios::app);
				test << client->mHeadBuffer << client->mBodyBuffer;
				test.close();
				// cout << "REQUEST ==============" << endl;
				// cout << client->mHeadBuffer << client->mBodyBuffer;
				// cout << "============================" << endl;
			}
			//END OF test code ========================================

			//make response to send
			response = client->getRequest()->createResponse();

			//test code (response check) ===============================
			{
				ofstream test("test_response.txt", ios::app);
				test << response;
				test.close();
				// cout << "RESPONSE ==============" << endl;
				// cout << response;
				// cout << "============================" << endl;
			}
			//END OF test code ========================================

			//event write queue add
			event->filter = EVFILT_WRITE;
			kevent(mKq, event, 1, NULL, 0, NULL);

			//1st send 요청 => 여기서 한 번 해둬야하는지 모르겠으나 일단 추가해둠
			client->setBuFlag(3);
			// send(client->getFd(), response.c_str(), response.size(), 0);
		} else if (client->getBuFlag() == 3) {
			cout << "SEND already started" << endl;
		}
	} else if (event->filter == EVFILT_WRITE) {
		size_t	sendinglen = client->getRequest()->mMSG.size() - client->getRequest()->mSendLen;
		if (static_cast<size_t>(event->data) <= sendinglen)
			sendinglen = event->data;

		send(client->getFd(), client->getRequest()->mMSG.c_str() + client->getRequest()->mSendLen, sendinglen, 0);
		client->getRequest()->mSendLen += sendinglen;
		if (client->getRequest()->mSendLen >= client->getRequest()->mMSG.size()) {
			//write done
			event->flags = EV_DELETE;
			kevent(mKq, event, 1, NULL, 0, NULL);
			client->setBuFlag(0);
			
		}

		
		
		// // send(client->getFd(), reinterpret_cast<void *>(&response), response.size(), 0);
		// send(client->getFd(), buffer, sizeof(buffer), 0);
		// cout << "EOF of send" << endl;

	} else {
		cout << "Handle Client: not read => " << event->filter << endl;
	}
}

void	SpiderMen::readSocket(struct kevent* event, Client* client)
{
		size_t	buffer_size = event->data;	
		char	buffer[buffer_size];
		memset(buffer, 0, buffer_size);
		ssize_t s = recv(client->getFd(), buffer, buffer_size, 0);
		

		if (s < 1) {
			if (s == 0)
			{
				// client->example();
				cout << "Client socket [" << client->getFd() << "] closed" << endl;
			}
			else
				cout << "Error: Client socket [" << client->getFd() << "] recv failed" << endl;
			close(client->getFd());	//연결 끊끊
			event->flags = EV_DELETE;
			kevent(mKq, event, 1, NULL, 0, NULL);

			//spidermen vector에서 client 삭제
			deleteClient(client->getFd());
			return ;
		}
		client->addBuffer(buffer, s);

		// cout << "buflag : " << client->getBuFlag() <<endl;

		// if (client->getBuFlag() == 2) {
		// 	string response = client->getRequest()->createResponse();
		// 	send(client->getFd(), response.c_str(), response.size(), 0);
		// 	close(client->getFd());	//연결 끊끊
		// 	event->flags = EV_DELETE;
		// 	kevent(mKq, event, 1, NULL, 0, NULL);
		// }
		
}