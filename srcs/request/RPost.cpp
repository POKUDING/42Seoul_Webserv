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

	//dir or file 체크
	// if (mIsFile)
	// 	throw 400;
	//method 사용가능한지 확인
	if (find(mLocation.getLimitExcept().begin(), mLocation.getLimitExcept().end(), "POST") == mLocation.getLimitExcept().end() && \
		find(mLocation.getLimitExcept().begin(), mLocation.getLimitExcept().end(), "PUT") == mLocation.getLimitExcept().end())
		throw 405;
}

RPost::~RPost() { }

// member functions

// public

const string	RPost::createResponse()
{
	string		mMSG;

	cout << "POST createResponse called\n";
	mMSG = mPipeValue;

	return mMSG;
}

pid_t	RPost::operate()
{
	cout << "POST operate called\n" << endl;
	mRequest = "POST";
	int	inFd[2];
	if (pipe(mPipe) == -1)
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
			throw 500;
		}
		close (inFd[1]);
		close (inFd[0]);
		setCgiEnv();
		executeCgi();
	}
	if (write(inFd[1], mBody.getBody().c_str(), mBody.getBody().size()) < 0)
	{
		cerr << "input to pipe error\n";
		throw 500;
	}
	close(inFd[0]);
	close(inFd[1]);
	close(mPipe[1]);
	return pid;
}

// private

void	RPost::executeCgi()
{
	// 표준 출력 rediretion (자식프로세스의 표준 출력을 mPipe의 write로)
	if (dup2(mPipe[1], STDOUT_FILENO) == -1) {
		write(mPipe[1], "dup2 error\n", 11);
		exit(EXIT_FAILURE);
	} 
	close (mPipe[1]);
	close (mPipe[0]);

	// 표준 입력 redirection (자식프로세스의 표준 입력을 생성한 파이프의 read로)

	// pipFd[1] 에 Body 쓰기

	// 환경변수 set/conf

	cerr << "cgi bin: " << getCgiBin() << endl;
	cerr << "cgi path: " << getCgiPath() << endl;
	string a = getCgiBin();
	string b;
	if (!a.size())
		a = getCgiPath();
	else
		b ="";
	char* const argv[3] = {const_cast<char * const>(a.c_str()), const_cast<char * const>(b.c_str()), NULL};
	// char* const argv[3] = {const_cast<char * const>(mLocation.getCgiBin().c_str()), const_cast<char * const>(mLocation.getCgiPath().c_str()), NULL};
	extern char** environ;
	for (int i = 0; i < 2; ++i)
		cerr << argv[i] << endl;
	setCgiEnv();
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