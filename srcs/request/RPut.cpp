#include "../../includes/request/RPut.hpp"

// constructor and destructor

RPut::RPut(string mRoot, map<string, string> header_key_val, vector<Server>* servers)
			: ARequest(mRoot, PUT, header_key_val, servers)
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
		// cerr << "content length fail" << endl;
		throw 400;
	}

	//file only인 location에 dir 요청으로 들어온 경우
	if (mLocation.getOnlyFile() && mIsFile ==  false) {
		// cerr << "/* only file Error" << endl;
		throw 400;
	}

	//method 사용가능한지 확인
	if (find(mLocation.getLimitExcept().begin(), mLocation.getLimitExcept().end(), "PUT") == mLocation.getLimitExcept().end())
		throw 405;
}

RPut::~RPut() { }

// member functions

// public

pid_t	RPut::operate()
{
	int	inFd[2];
	int	outFd[2];

	if (pipe(outFd) == -1)
		throw 500;
	if (pipe(inFd) == -1)
		throw 500;
	pid_t pid = fork();
	if (pid == -1)
		throw 500;
	else if (pid == 0) {
		if (dup2(inFd[0], STDIN_FILENO) == 1) {
			exit (EXIT_FAILURE);
		}
		if (dup2(outFd[1], STDOUT_FILENO) == -1) {
			exit(EXIT_FAILURE);
		}
		if (close(outFd[1]) == -1) {
			exit(EXIT_FAILURE);
		}
		if (close(outFd[0]) == -1) {
			exit(EXIT_FAILURE);
		}
		if (close(inFd[1]) == -1) {
			exit(EXIT_FAILURE);
		}
		if (close(inFd[0]) == -1) {
			exit(EXIT_FAILURE);
		}
		executeCgi();
	}
	mWritePipe = inFd[1];
	mReadPipe = outFd[0];
	if (fcntl(mWritePipe, F_SETFL, O_NONBLOCK) == -1)
		throw 500;
	if (fcntl(mReadPipe, F_SETFL, O_NONBLOCK) == -1)
		throw 500;
	if (close(inFd[0]) == -1)
		throw 500;
	if (close(outFd[1]) == -1)
		throw 500;
	return pid;
}

const string	RPut::createResponse()
{
	string 	mMSG;
	char	timeStamp[1024];

	mMSG.append(STATUS_200);
	Time::stamp(timeStamp);
	mMSG.append(timeStamp);
	mMSG.append(SPIDER_SERVER);
	if (this->getBasics().connection == KEEP_ALIVE)
		mMSG.append("Connection: keep-alive\r\n");
	mMSG.append("Content-Length: ");
	if (mPipeValue.find("\r\n\r\n") != string::npos) {
		mMSG.append(SpiderMenUtil::itostr(mPipeValue.size() - (mPipeValue.find("\r\n\r\n") + 4)).c_str());
		mMSG.append("\r\n");
		mMSG.append(mPipeValue);
	} else
	{
		mMSG.append("0\r\n\r\n");
		// cerr << "mPipeValue cannot found CRLF" << endl;
	}

	return mMSG;
}

// private

void	RPut::executeCgi()
{
	extern char** environ;
	char* const argv[2] = {const_cast<char * const>(mCgiPath.c_str()), NULL};
	setCgiEnv();
	if (execve(argv[0], argv, environ) == -1)
		cerr << "RPUT: excute falied error\n";
	exit(EXIT_FAILURE);
}
