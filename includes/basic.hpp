#ifndef BASIC_HPP
#define BASIC_HPP

#define SERVER_PORT 12345
#define MAX_BODY_SIZE 100000
#define MAX_PORT_SIZE 65535		//max(short)
#define DEFAULT_FILE "./conf/default.conf"
#define BACKLOG 15
#define MAX_EVENT 2048
#define PACKET_SIZE 1460

namespace nSocket {
	enum eType {
		SERVER = 0,
		CLIENT = 1
	};
}

#endif //BASIC_HPP