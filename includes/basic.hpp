#ifndef BASIC_HPP
#define BASIC_HPP

#define SERVER_PORT 12345
#define MAX_BODY_SIZE 100000
#define MAX_PORT_SIZE 65535		//max(short)
#define DEFAULT_FILE "./conf/default.conf"
#define BACKLOG 15
#define MAX_EVENT 2048
#define PACKET_SIZE 1460

#define HTTP_STATUS "HTTP/1.1 200 OK\r\n"
#define SPIDER_SERVER "Server: SpiderMen/1.0.0\r\n"
#define CONTENT_TYPE "Content-Type: text/html; charset=UTF-8\r\n"

// #define OK "OK"
// #define BAD_REQUEST "Bad Request"
// #define SERVER_ERROR "Internal Server Error"

#define TIMEOUT_SEC 10

#define CHUNKED -1
#define FAIL_FD 0

namespace nSocket {
	enum eType {
		SERVER = 0,
		CLIENT = 1
	};

	enum eConnection {
		CLOSE = 0,
		KEEP_ALIVE = 1
	};
}

namespace nMethod {
	enum eType {
		GET = 0,
		POST = 1,
		DELETE = 2
	};
}

namespace nStatus {
	enum eClient {
		WAITING = 0,		//(writing done), connected & waiting
		READING_HEADER = 1,	//request start, reading header
		READING_BODY = 2,	//header done, reading body
		PROCESSING = 3,		//body done(request recv done), processing
		SENDING = 4			//writing
	};

}

#endif //BASIC_HPP