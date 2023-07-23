#ifndef BASIC_HPP
#define BASIC_HPP

#define SERVER_PORT 12345
#define MAX_BODY_SIZE 100000
#define MAX_PORT_SIZE 65535		//max(short)
#define DEFAULT_FILE "./conf/default.conf"
#define BACKLOG 15
#define MAX_EVENT 2048
#define PACKET_SIZE 1460

#define HTTP_VERSION "HTTP/1.1 "
#define SPIDER_SERVER "Server: SpiderMen/1.0.0 (MAC OS)\r\n"
#define CONTENT_TYPE "Content-Type: text/html; charset=UTF-8\r\n"

namespace nSocket {
	enum eType {
		SERVER = 0,
		CLIENT = 1
	};
}

namespace nMethod {
	enum eType {
		GET = 0,
		POST = 1,
		DELETE = 2
	};
}

#endif //BASIC_HPP