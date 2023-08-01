#include "../../includes/request/RCgi.hpp"

// constructor and destructor

RCgi::RCgi(string mRoot, map<string, string> header_key_val, vector<Server>* servers)
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

RCgi::~RCgi() { }

// member functions

// public

const string	RCgi::createResponse()
{
	string		mMSG;

	cout << "POST createResponse called\n";
	mMSG = mPipeValue;

	return mMSG;
}

pid_t	RCgi::operate()
{
	cout << "POST operate called\n" <<endl;
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
			cerr << "dup2 error\n" << endl;
			throw 500;
		}
		close (inFd[1]);
		close (inFd[0]);
		setCgiEnv();
		executeCgi();
	}
	if (write(inFd[1], mBody.getBody().c_str(), mBody.getBody().size()) < 0)
	{
		cerr << "input to pipe error\n" << endl;
		throw 500;
	}
	close(inFd[0]);
	close(inFd[1]);
	close(mPipe[1]);
	return pid;
}

// private

void	RCgi::executeCgi()
{
	// 표준 출력 rediretion (자식프로세스의 표준 출력을 mPipe의 write로)
	if (dup2(mPipe[1], STDOUT_FILENO) == -1) {
		write(mPipe[1], "dup2 error\n", 11);
		exit(EXIT_FAILURE);
	} 
	close (mPipe[1]);
	close (mPipe[0]);

	char* const argv[2] = {const_cast<char * const>(mLocation.getCgiPath().c_str()), NULL};
	extern char** environ;
	setCgiEnv();
	if (execve(argv[0], argv, environ) == -1)
		cerr << "excute falied error\n";
	// write(STDOUT_FILENO, "execve error\n", 12);
	exit(EXIT_FAILURE);
}

string RCgi::getRequestMethod()
{
	if (mType == GET)
		return "GET";
	else if (mType == POST)
		return "POST";
	else // (mType == DELETE)
		return "DELETE";
}

// getters and setters

// const Body&	RCgi::getBody() const { return mBody; }