#ifndef BASIC_HPP
#define BASIC_HPP

#define MAX_BODY_SIZE 100000000
#define MAX_PORT_SIZE 65535		//max(short)
#define DEFAULT_CONF_FILE "./conf/default.conf"
#define DEFAULT_ERROR_PAGE "./www/errors/error_default.html"
#define BACKLOG 1000
#define MAX_EVENT 2048
#define PACKET_SIZE 1460

#define STATUS_200 "HTTP/1.1 200 OK\r\n"
#define STATUS_201 "HTTP/1.1 201 Created\r\n"
#define STATUS_301 "HTTP/1.1 301 Moved Permanently\r\n"
#define STATUS_400 "HTTP/1.1 400 Bad Request\r\n"
#define STATUS_403 "HTTP/1.1 403 Forbidden\r\n" 
#define STATUS_404 "HTTP/1.1 404 Not Found\r\n"
#define STATUS_405 "HTTP/1.1 405 Method Not Allowed\r\n"
#define STATUS_413 "HTTP/1.1 413 Request Entity Too Large\r\n"
#define STATUS_500 "HTTP/1.1 500 Internal Server Error\r\n"
#define STATUS_501 "HTTP/1.1 501 Not Implemented\r\n"
#define STATUS_504 "HTTP/1.1 504 Gateway Time-out\r\n"
#define STATUS_505 "HTTP/1.1 505 HTTP Version not supported\r\n"

#define SPIDER_SERVER "Server: SpiderMen/1.2.3\r\n"
#define CONTENT_HTML "Content-Type: text/html; charset=UTF-8\r\n"

#define CLOSE "close"
#define KEEP_ALIVE "keep-alive"

#define TIMEOUT_SEC 10

#define CHUNKED -1
#define FAIL_FD 0

enum eSocket {
	SERVER = 0,
	CLIENT = 1
};

enum eMethod {
	GET = 0,
	POST = 1,
	DELETE = 2,
	PUT = 3,
	BAD = 4,
	HEAD = 5
};

enum eReadStatus {
	WAITING = 0,		//(writing done), connected & waiting
	READING_HEADER = 1,	//request start, reading header
	READING_BODY = 2,	//header done, reading body
	ERROR = 3			//something wrong, no more reading
};

enum eRequestStatus {
	EMPTY = 0,			//no requests
	PROCESSING = 1,		//cgi working
	SENDING = 2			//writing
};

#endif // BASIC_HPP