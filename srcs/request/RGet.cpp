#include "../../includes/request/RGet.hpp"

//  constructor and destructor

RGet::RGet(string mRoot, map<string, string> header_key_val, vector<Server>* servers)
			: ARequest(mRoot, GET, header_key_val, servers)
{
	mMethod = "GET";
	if (mBasics.content_length || mBasics.transfer_encoding.size())
		throw 400;

	//해당 location 블록에서 method 사용가능한지 확인
	if (find(mLocation.getLimitExcept().begin(), mLocation.getLimitExcept().end(), "GET") == \
			mLocation.getLimitExcept().end())
		throw 405;
}

RGet::~RGet() { }

// member functions

// public

pid_t	RGet::operate()
{
	//dir일 경우, index page가 있으면 root에 붙여준다 && mIsFile = 1
	//아니면 그대로 진행
	if (mIsFile ==  false) {
		if (mLocation.getIndex().size()) {
			mRoot += "/" + mLocation.getIndex();
			mIsFile = true;
		}
	}

	if (mIsFile == true) {
		//.php, .py 블록이면 CGI 처리
		if (mCgiPath.size()) {

			int outFd[2];
			setPipe(outFd);	//pipe 생성 (ARequest에 있음)
			pid_t pid = fork();

			//cgi 실행 파일, 전달할 .py/php 파일 이름, NULL
			if (pid == 0) {
				if (dup2(outFd[1], STDOUT_FILENO) == -1) {
					exit(EXIT_FAILURE);
				}
				if (close (outFd[0]) == -1) {
					exit(EXIT_FAILURE);
				}
				if (close (outFd[1]) == -1) {
					exit(EXIT_FAILURE);
				}
				executeCgi();
			}
			mWritePipe = 0;
			mReadPipe = outFd[0];
			if (fcntl(mReadPipe, F_SETFL, O_NONBLOCK) == -1) {
				throw 500;
			}
			if (close(outFd[1]) == -1) {
				throw 500;
			}
			return pid;
		}
	}
	return 0;
}

const string	RGet::createResponse()
{
	if (mLocation.getRedirect().size())
		return createRedirectResponse();
	else if (getCgiPath().size())
		return createCgiResponse();
	else
		return createLegacyResponse();
}

void	RGet::executeCgi()
{
	extern char** environ;
	setCgiEnv();
	char* const argv[2] = {const_cast<char * const>(mCgiPath.c_str()), NULL};
	if (execve(argv[0], argv, environ) < 0) {
		cerr << "execve failed!!" << strerror(errno) << endl;
	}
	exit(EXIT_FAILURE);
}

// private

const string	RGet::createRedirectResponse()
{
	string	mMSG;

	mMSG.append(STATUS_301);
	mMSG.append("Location: ");
	mMSG.append(mLocation.getRedirect());
	mMSG.append("\r\nContent-Length: 0\r\n\r\n");

	return mMSG;
}

const string	RGet::createCgiResponse()
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
	} else {
		mMSG.append("0\r\n\r\n");
		// cout << "mPipeValue cannot found CRLF" << endl;
	}

	return mMSG;
}

const string	RGet::createLegacyResponse()
{
	string		mMSG;
	char 		timeStamp[TIME_SIZE];
	stringstream to_str;
	stringstream tmp;
	string		buffer;
	string		body;

	//1st line: STATUS
	mMSG.append(STATUS_200);	//"HTTP/1.1 200 OK\r\n"

	//HEADER============================================
	Time::stamp(timeStamp);
	mMSG.append(timeStamp);		//Date: Tue, 20 Jul 2023 12:34:56 GMT\r\n
	mMSG.append(SPIDER_SERVER);	//Server: SpiderMen/1.0.0\r\n

	if (mLocation.getRedirect().size())
		return createRedirectResponse();
	if (mIsFile == true) {

		ifstream	fin(getRoot());
		if (fin.fail())
			throw 404;
		tmp << fin.rdbuf();
		body = tmp.str();
		fin.close();

		size_t	pos = mRoot.rfind('.');
		Mime	mime;
		string	extension = mRoot.substr(pos);

		if (pos == string::npos || mime[extension].size() == 0)
			mMSG.append(CONTENT_PLAIN);
		else {
			mMSG.append("Content-Type: ");
			mMSG.append(mime[extension]);
			mMSG.append("\r\n");
		}

		mMSG.append("Content-Length: ");
		// cout << "body size = " << body.size() << endl;
		to_str << body.size();
		to_str >> buffer;
		// cout << "body size  in buffer = " << buffer << endl;
		mMSG.append(buffer);
		mMSG.append("\r\n");

		if (this->getBasics().connection == KEEP_ALIVE)
			mMSG.append("Connection: keep-alive\r\n");

		mMSG.append("\r\n"); //end of head

		//BODY 추가
		mMSG.append(body.c_str(), body.size());

	} else {//if dir
		mMSG.append(CONTENT_PLAIN);

		if (mLocation.getAutoIndex()) {// autoindex yes => 리스트 보여줌

			DIR* dir = opendir(mRoot.c_str());
			if (!dir)
				throw runtime_error("Error: GET autoindex read failed");
			dirent * file;

			file = readdir(dir);	// .
			file = readdir(dir);	// ..
			while ((file = readdir(dir)) != NULL) {
				body.append(file->d_name);
				if (file->d_type & DT_DIR)
					body.append("/");
				body.append("\r\n");
			}

			if (body.size() == 0) {
				mMSG.append("no file\r\n");
			}

			mMSG.append("Content-Length: ");
			to_str << body.size();
			to_str >> buffer;
			mMSG.append(buffer + "\r\n\r\n");
			mMSG.append(body);

		} else {// autoindex np => 뭘 보여주지
			// mMSG.clear();
			// mMSG.append(STATUS_404);
			// mMSG.append(timeStamp);		//Date: Tue, 20 Jul 2023 12:34:56 GMT\r\n
			// mMSG.append(SPIDER_SERVER);	//Server: SpiderMen/1.0.0\r\n
			// mMSG.append("Content-Type: text/plain\r\n");

			//원본
			mMSG.append("Content-Length: 24\r\n\r\n");
			mMSG.append("Auto-Index not available");
		}
	}

	return mMSG;
}
