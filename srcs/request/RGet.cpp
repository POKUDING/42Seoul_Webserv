#include "../../includes/request/RGet.hpp"

//  constructors and destructor

RGet::RGet(string mRoot, map<string, string> header_key_val, vector<Server>* servers)
			: ARequest(mRoot, GET, header_key_val, servers)
{
	mRequest = "GET";
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

pid_t			RGet::operate()
{
	if (!mIsFile) {//dir일 경우
		if (mRoot == "/") {
			mRoot = "./www/index.html";
			mIsFile = true;
		} else if (mRoot == mLocation.getKey()) {//정확히 일치하는 block이면
			string defaultIndex = mRoot + "/index.html";
			mIsFile = true;
			if (mLocation.getIndex().size()) {
				mRoot = mLocation.getIndex();
			} else if (access(defaultIndex.c_str(), F_OK) == 0) {
				mRoot = defaultIndex;
			} else {
				mIsFile = false;
			}
		} else {
			string defaultIndex = mRoot + "/index.html";
			if (access(defaultIndex.c_str(), F_OK) == 0) {
				mRoot = defaultIndex;
				mIsFile = true;
			}
		}
	}

	if (mIsFile) {
		
		//.php, .py 블록이면 CGI 처리
		if (mLocation.getCgiPath().size()) {
			
			setPipe();	//pipe 생성 (ARequest에 있음)
			pid_t pid = fork();

			//cgi 실행 파일, 전달할 .py/php 파일 이름, NULL
			if (pid == 0) {
				extern char** environ;
				setCgiEnv();
				cout << "\n\n\n\n\n" <<mRequest << "\n\n\n\n" << endl;
				char* const argv[3] = {const_cast<char * const>(mLocation.getCgiPath().c_str()), \
										const_cast<char * const>(getRoot().c_str()), NULL};
				for (int i = 0; i < 2; ++i)
					cout << argv[i]<< endl;
				cout << mLocation.getCgiBin() << endl;
				dup2(mPipe[1],1);
				close (mPipe[0]);
				close (mPipe[1]);
				if (execve(mLocation.getCgiBin().c_str(), argv, environ) < 0)
				{
					cerr<< "execve faile!!" << strerror(errno) << endl;
					exit(1);
				}
			}
			close(mPipe[1]);
			return pid;
		} 
	}
	return 0;
}

const string	RGet::redirectResponse()
{
	string	mMSG;

	cout << "redirect MSG !" << endl;
	mMSG.append("HTTP/1.1 301 Moved Permanently\r\n");
	mMSG.append("Location: ");
	mMSG.append(mLocation.getRedirect());
	mMSG.append("\r\nContent-Length: 0\r\n\r\n");

	return mMSG;
}

const string	RGet::createResponse()
{
	cout << "create Response !" << endl;
	if (mLocation.getRedirect().size())
		return redirectResponse();
	else if (mLocation.getCgiPath().size())
		return createCgiResponse();
	else
		return createLegacyResponse();	
}

const string	RGet::createCgiResponse()
{
	string 	mMSG;
	char	timeStamp[1024];

	cout << mPipeValue << endl;
	mMSG.append("HTTP/1.1 200 OK\r\n");
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
		mMSG.append("\r\n\r\n");
	} else
	{
		mMSG.append("0");
		cout << "mPipeValue cannot found CRLF" << endl;
	}

	return mMSG;
}

const string	RGet::createLegacyResponse()
{
	// if (checkCgi)
	// 	return cgiResoponse();
	// else
	//	return commonResponse();

	cout << "create Legacy RESPONSE!" << endl;

	string		mMSG;
	char 		timeStamp[TIME_SIZE];
	stringstream to_str;
	stringstream tmp;
	string		buffer;
	string		body;

	//1st line: STATUS
	mMSG.append(HTTP_STATUS);	//"HTTP/1.1 200 OK\r\n"
	
	//HEADER============================================
	Time::stamp(timeStamp);
	mMSG.append(timeStamp);		//Date: Tue, 20 Jul 2023 12:34:56 GMT\r\n
	mMSG.append(SPIDER_SERVER);	//Server: SpiderMen/1.0.0\r\n

	if (mLocation.getRedirect().size())
		return redirectResponse();
	if (mIsFile) {
		
		ifstream	fin(getRoot());
		if (fin.fail())
			throw runtime_error("Error: GET body file read failed");
		tmp << fin.rdbuf();
		body = tmp.str();
		fin.close();

	//MIME 처리 필요
		mMSG.append(CONTENT_TYPE);	//Content-Type: text/html; charset=UTF-8\r\n
		// mMSG.append("Content-Type: ");	//png 등의 경우 별도의 content-type필요
		
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
		mMSG.append("Content-Type: text/plain\r\n");
		mMSG.append("Content-Length: ");
		cout << mMSG << "=====" << endl;
		if (mLocation.getAutoIndex()) {// autoindex yes => 리스트 보여줌
			
			DIR* dir = opendir(mRoot.c_str());
			if (!dir)
				throw runtime_error("Error: GET autoindex read failed");
			dirent * file;

			file = readdir(dir);
			file = readdir(dir);
			while ((file = readdir(dir)) != NULL) {
				body.append(file->d_name);
				if (file->d_type & DT_DIR)
					body.append("/");
				body.append("\r\n");
			}

			if (body.size() == 0) {
				mMSG.append("no file\r\n");
			}

			to_str << body.size();
			to_str >> buffer;
			mMSG.append(buffer + "\r\n\r\n");
			mMSG.append(body);

		} else {// autoindex np => 뭘 보여주지
			mMSG.append("Content-Length: 24\r\n\r\n");
			mMSG.append("Auto-Index not available");
		}
	}

	return mMSG;
}
