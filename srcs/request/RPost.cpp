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
		cout << "content length fail" << endl;
		throw 400;
	}

	//file only인 location에 dir 요청으로 들어온 경우
	if (mLocation.getOnlyFile() && !mIsFile) {
		throw 400;
	}
	
	//method 사용가능한지 확인
	if (find(mLocation.getLimitExcept().begin(), mLocation.getLimitExcept().end(), "POST") == mLocation.getLimitExcept().end() && \
		find(mLocation.getLimitExcept().begin(), mLocation.getLimitExcept().end(), "PUT") == mLocation.getLimitExcept().end())
		throw 405;
}

RPost::~RPost() { }

// member functions

// public

pid_t	RPost::operate()
{
	cout << "POST operate called\n" << endl;
	mMethod = "POST";
	int	inFd[2];
	int	outFd[2];

	if (pipe(outFd) == -1)
		throw runtime_error("Error: pipe error");
	if (pipe(inFd) == -1)
		throw runtime_error("Error: pipe error");
	pid_t pid = fork();
	if (pid == -1)
		throw runtime_error("Error: fork error");
	else if (pid == 0) {
		if (dup2(inFd[0], STDIN_FILENO) == 1)
		{
			cerr << "dup2 error\n";
			exit (EXIT_FAILURE);
		}
		if (dup2(outFd[1], STDOUT_FILENO) == -1) {
			cerr << "dup2 error" << endl;
			exit(EXIT_FAILURE);
		} 
		close (outFd[1]);
		close (outFd[0]);
		close (inFd[1]);
		close (inFd[0]);
		setCgiEnv();
		executeCgi();
	}
	mWritePipe = inFd[1];
	mReadPipe = outFd[0];
	close(inFd[0]);
	close(outFd[1]);
	return pid;
}

const string	RPost::createResponse()
{
	string 	mMSG;
	char	timeStamp[1024];

	// cout << mPipeValue << endl;
	mMSG.append(STATUS_201);
	Time::stamp(timeStamp);
	mMSG.append(timeStamp);	
	mMSG.append(SPIDER_SERVER);
	if (this->getBasics().connection == KEEP_ALIVE)
		mMSG.append("Connection: keep-alive\r\n");
	mMSG.append("Content-Length: ");
	if (mPipeValue.find("\r\n\r\n") != string::npos) {
		mMSG.append(SpiderMenUtil::itostr(mPipeValue.size() - (mPipeValue.find("\r\n\r\n") + 4)).c_str());
		mMSG.append("\r\n\r\n");
	} else
	{
		mMSG.append("0");
		cerr << "mPipeValue cannot found CRLF" << endl;
	}

	return mMSG;
}

// private

void	RPost::executeCgi()
{
	// 표준 출력 rediretion (자식프로세스의 표준 출력을 mPipe의 write로)

	// 표준 입력 redirection (자식프로세스의 표준 입력을 생성한 파이프의 read로)

	// pipFd[1] 에 Body 쓰기

	// 환경변수 set/conf

	cerr << "cgi bin: " << getCgiBin() << endl;
	cerr << "cgi path: " << getCgiPath() << endl;
	// char* const argv[3] = {const_cast<char * const>(mLocation.getCgiBin().c_str()), const_cast<char * const>(mLocation.getCgiPath().c_str()), NULL};
	extern char** environ;
	setCgiEnv();
	char* const argv[2] = {const_cast<char * const>(mCgiBin.c_str()), NULL};
	for (int i = 0; i < 1; ++i)
		cerr << argv[i] << endl;
	if (execve(argv[0], argv, environ) == -1)
		cerr << "excute falied error\n";
	exit(EXIT_FAILURE);
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