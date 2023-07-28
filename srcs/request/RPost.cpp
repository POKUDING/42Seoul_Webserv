#include "../../includes/request/RPost.hpp"

// constructor and destructor

RPost::RPost(string mRoot, map<string, string> header_key_val, vector<Server>* servers)
			: ARequest(mRoot, POST, header_key_val, servers)
{
	if (mBasics.content_length) {
		size_t	pos;
		
		pos = mBasics.content_type.find("boundary=");
		mBody.setContentLen(mBasics.content_length);
		if (pos != string::npos)
			mBasics.boundary = mBasics.content_type.substr(pos + 9, mBasics.content_type.size() - pos - 9);
	} else if (mBasics.transfer_encoding == "chunked") {
		mBody.setChunked(true);
	} else {
		throw runtime_error("Bad request:: Post:: no content-length");
	}

	//dir or file 체크
	if (mIsFile)
		throw runtime_error("Bad request:: Post:: not valid directory");
	//method 사용가능한지 확인
	if (find(mLocation.getLimitExcept().begin(), mLocation.getLimitExcept().end(), "POST") == \
			mLocation.getLimitExcept().end())
		throw runtime_error("Bad request:: Post:: method not available");
}

RPost::~RPost() { }

// member functions

// public

const string	RPost::createResponse()
{
	string		mMSG;
	char		buff[1024];
	size_t 		readLen = 1;

	cout << "POST createResponse called\n";
	close(mPipe[1]);
	while (readLen > 0)
	{
		readLen = read(mPipe[0], buff, 1024);
		mMSG.append(buff, readLen);
	}
	close(mPipe[0]);
	mPipe[0] = 0;
	mPipe[1] = 0;

	cout << mMSG << endl;
	return mMSG;
}

pid_t	RPost::operate()
{
	cout << "POST operate called\n";
	if (pipe(mPipe) == -1)
		throw runtime_error("Error: pipe error");
	pid_t pid = fork();
	if (pid == -1)
		throw runtime_error("Error: fork error");
	else if (pid == 0) {
		setCgiEnv();
		executeCgi();
	}
	return pid;
}

// private

void	RPost::executeCgi()
{
// 표준 출력 rediretion (자식프로세스의 표준 출력을 mPipe의 write로)
	// if (close(mPipe[0]) == -1) {
	// 	write(mPipe[1], "close fd error\n", 13);
	// 	exit(EXIT_FAILURE);
	// }
	if (dup2(mPipe[1], STDOUT_FILENO) == -1) {
		write(mPipe[1], "dup2 error\n", 11);
		exit(EXIT_FAILURE);
	} 
	// if (close(mPipe[1]) == -1) {
	// 	write(mPipe[1], "close fd error\n", 13);
	// 	exit(EXIT_FAILURE);
	// } // redirection 끝  굳이 close 안 해도 될 듯
	int pipeFd[2]; // body를 표준 입력에 쓰기
	if (pipe(pipeFd) == -1) {
		write(STDOUT_FILENO, "pipe error\n", 11);
		exit(EXIT_FAILURE);
	}
	// 표준 입력 redirection (자식프로세스의 표준 입력을 생성한 파이프의 read로)
	if (dup2(pipeFd[0], STDIN_FILENO) == 1) {
		write(STDOUT_FILENO, "dup2 error\n", 11);
		exit(EXIT_FAILURE);
	}
	// pipFd[1] 에 Body 쓰기
	write(pipeFd[1], mBody.getBody().c_str(), mBody.getBody().size());
	if (close(pipeFd[1]) == -1) {
		write(mPipe[1], "close fd error\n", 13);
		exit(EXIT_FAILURE);
	}
	// 환경변수 set

	char* const argv[3] = {const_cast<char * const>(mLocation.getCgiBin().c_str()), const_cast<char * const>(mLocation.getCgiPath().c_str()), NULL};
	extern char** environ;
	setCgiEnv();
	execve(argv[0], argv, environ);
	write(STDOUT_FILENO, "execve error\n", 12);
	exit(EXIT_FAILURE);
}

void	RPost::setCgiEnv()
{
	// setenv("SERVER_NAME", getBasics().host.c_str(), 1);
	setenv("CONTENT_TYPE", getBasics().content_type.c_str(), 1);
	setenv("CONTENT_LENGTH", SpiderMenUtil::itostr(getBasics().content_length).c_str(), 1);
	// setenv("HTTP_USER_AGENT", getBasics().user_agent.c_str(), 1);
	// setenv("SERVER_SOFTWARE", SPIDER_SERVER, 1); // "\r\n 빼야하나?"
	// setenv("REQUEST_METHOD", getRequestMethod().c_str(), 1);
	// setenv("PATH_INFO", mLocation.getCgiPath().c_str(), 1);
	// setenv("SERVER_PORT", to_string(mServer.getListen()).c_str(), 1);
	// setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);

	// // SERVER_NAME
	// // SERVER_SOFTWARE // CGI 프로그램 이름 및 버전 ex) 아파치 / 2.2.14
	// SERVER_PROTOCOL // ??? HTTP/1.1 ?
	// // SERVER_PORT // ???
	// // REQUEST_METHOD // GET POST ..
	// // PATH_INFO // CGI path /wevsrv/cgi_bin/example.cgi
	// DOCUMENT_ROOT // ???
	// QUERY_STRING // url?key=value&key=value ..
	// REMOTE_HOST	//client host name 없으면 정의 x
	// REMOTE_ADDR	//client ip
	// // CONTENT_TYPE
	// // CONTENT_LENGTH
	// HTTP_REFERER // ????
	// // HTTP_USER_AGENT
}

string RPost::getRequestMethod()
{
	if (mType == GET)
		return "GET";
	else if (mType == POST)
		return "POST";
	else // (mType == DELETE)
		return "DELETE";
}

// getters and setters

// const Body&	RPost::getBody() const { return mBody; }