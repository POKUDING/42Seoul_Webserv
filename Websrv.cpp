#include "Websrv.hpp"

void	Websrv::initSocket(const vector<Server>& servers)
{
	int		fd_tmp;
	struct	sockaddr_in server_addr;

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;					//IPv4 인터넷 프로토콜
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	//호스트 주소 자동할당옵션
	for(size_t i = 0; i < servers.size(); i++)
	{
		//socket 생성
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
		this->fd_servers.insert(pair<int,const Server&>(fd_tmp, servers[i]));
	}

	// cout << "start" << endl;
	// for (map<int,const Server&>::iterator it = fd_servers.begin(); it != fd_servers.end(); ++it)
	// 	cout << it->first << ": " << it->second.getListen() << endl;
	// cout << "end" << endl;
}

Websrv::Websrv(const Config& config)
{
	this->initSocket(config.getServer());
}

Websrv::~Websrv(void)
{
	for(map<int,const Server&>::iterator it = fd_servers.begin(); it != fd_servers.end(); it++)
		close(it->first);
}