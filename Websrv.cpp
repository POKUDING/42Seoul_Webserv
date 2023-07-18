#include "Websrv.hpp"

// constructors and destructor

Websrv::Websrv(const Config& config)
{
	this->initSocket(config.getServer());
}

Websrv::~Websrv()
{
	for (map<int,const Server&>::iterator it = mFdServers.begin(); it != mFdServers.end(); it++)
		close(it->first);

	if (mKq)	//kq 초기값이 없어서 이거 애매할수도
		close(mKq);
}

// member functions

void	Websrv::initSocket(const vector<Server>& servers)
{
	int		fd_tmp;
	struct	sockaddr_in server_addr;

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;					//IPv4 인터넷 프로토콜
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	//호스트 주소 자동할당옵션
	for(size_t i = 0; i < servers.size(); i++)
	{
		//(socket 상속) server socket 생성
		//server socket 확인용 map은 필요없어지는가요? 그런가요


		fd_tmp = socket(AF_INET, SOCK_STREAM, 0);
		if (fd_tmp == -1)
			throw runtime_error("Error: socket open failed.");
		server_addr.sin_port = htons(servers[i].getListen());	//사용할 포트 (hton short로 변경한 값 저장)
		if (bind(fd_tmp, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) == -1)
			throw runtime_error("Error: socket bind failed.");
		if (listen(fd_tmp, BACKLOG) == -1)
			throw runtime_error("Error: socket listen failed.");
		if (fcntl(fd_tmp, F_SETFL, O_NONBLOCK) == -1)			//소켓 논블록처리
			throw runtime_error("Error: socket nonblock failed.");
		this->mFdServers.insert(pair<int,const Server&>(fd_tmp, servers[i]));
	}

	// cout << "start" << endl;
	// for (map<int,const Server&>::iterator it = mFdServers.begin(); it != mFdServers.end(); ++it)
	// 	cout << it->first << ": " << it->second.getListen() << endl;
	// cout << "end" << endl;
}

void	Websrv::initKque()
{
	struct kevent	event;
	mKq = kqueue();
	if (mKq == -1)
		throw runtime_error("Error: kqueue create failed.");
	for (map<int,const Server&>::iterator it = mFdServers.begin(); it != mFdServers.end(); it++) {
		
		//EV_SET에 server socket 추가 (NULL 변경)
		
		EV_SET(&event, it->first, EVFILT_READ, EV_ADD, 0, 0, NULL);
	    if (kevent(mKq, &event, 1, NULL, 0, NULL) == -1)
			throw runtime_error("Error: kevent add failed.");
	}
}

void	Websrv::run()
{
	struct kevent events[MAX_EVENT];

	this->initKque();
	int event_num;
	while (1) {
		event_num = kevent(mKq, NULL, 0, events, MAX_EVENT, NULL);
		if (event_num == -1) {
			//에러 발생하면 해당 건만 error log 등으로 별도 처리,
			//웹서버는 그대로 진행되어야 함 (혹은 재시작?)
			throw runtime_error("Error: Kqueue event detection failed");
		}

		//events에 있는 socket으로 type, fd 확인가능해짐

		for (int i = 0; i < event_num; ++i) {
			if (events[i].filter == EV_ERROR) {
				//에러 발생하면 해당 건만 error log 등으로 별도 처리,
				//웹서버는 그대로 진행되어야 함 (혹은 재시작?)
				throw runtime_error("Error: event filter error");
			}
			

			// 1) event가 server socket 인 경우 (new_client 등록 요청)
			// if (events[i].udata.type == SERVER)
			for (map<int,const Server&>::iterator it = mFdServers.begin(); it != mFdServers.end(); ++it)
			{
				// 1) event가 server socket 인 경우 (new_client 등록 요청)
				if (events[i].ident == static_cast<uintptr_t>(it->first)) {

					Client client_tmp;
					struct kevent event_tmp;
					struct sockaddr_in client_addr;
					
					// 새로운 연결 요청 수락
					socklen_t client_len = sizeof(client_addr);
					client_tmp.setFd(accept(it->first, reinterpret_cast<struct sockaddr*>(&client_addr), &client_len));
					if (client_tmp.getFd() == -1)
						throw runtime_error("Error: client accept failed");
					
					// 새로운 클라이언트 소켓을 kqueue에 등록
					EV_SET(&event_tmp, client_tmp.getFd(), EVFILT_READ, EV_ADD, 0, 0, NULL);
					if (kevent(mKq, &event_tmp, 1, NULL, 0, NULL) == -1)
						throw runtime_error("Error: client kevent add error");
					mClients.push_back(client_tmp);
				}
			}

			// 2) event가 client socket 인 경우
			// else if (events[i].udata.type == CLIENT)
			for (vector<Client>::iterator it = mClients.begin(); it !=  mClients.end(); ++it)
			{
				if (events[i].ident == it->getFd())
					{
					// 2) event가 client socket 인 경우
						// 2-1) 연결 끊김 (이게 client socket으로 오는지 봐야함)
					
						// 2-2) request 진행
							//request 읽기 시작 => recv() 요청
							//request 읽기 완료 => recv() return 값 받아서 진행
							//request 파싱	   => request parsing
							//(parsing 결과에 따라) ARequest 객체 생성
							//				  => (객체 내에서 추가 parsing 진행?)
							//				  
						
					}
			}
			
					
			

		}
	}
	 

	
}