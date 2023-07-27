#include "../../includes/request/RGet.hpp"

//  constructors and destructor

RGet::RGet(string mRoot, map<string, string> header_key_val, vector<Server>* servers)
			: ARequest(mRoot, GET, header_key_val, servers)
{
	if (mBasics.content_length || mBasics.transfer_encoding.size()) 
		throw runtime_error("Bad request:: GET cannot have body");
	
	//해당 location 블록에서 method 사용가능한지 확인
	if (find(mLocation.getLimitExcept().begin(), mLocation.getLimitExcept().end(), "GET") == \
			mLocation.getLimitExcept().end())
		throw runtime_error("Bad request:: Get:: method not available");	
}

RGet::~RGet() { }

// member functions

// public

pid_t			RGet::operate()
{
	if (mIsFile) {
		//.php, .py 블록이면 CGI 처리
		if (mLocation.getKey() == ".php$" || mLocation.getKey() == ".py$") {
			
			setPipe();	//pipe 생성 (ARequest에 있음)
			pid_t pid = fork();

			//cgi 실행 파일, 전달할 .py/php 파일 이름, NULL
			char* const argv[3] = {const_cast<char * const>(mLocation.getFastcgiPass().c_str()), \
									const_cast<char * const>(getRoot().c_str()), NULL};
			if (pid == 0) {
				if (mLocation.getKey() == ".php$")
					execve("/usr/bin/php", argv, env);
				else//".py$"
					execve("/usr/local/bin/python3", argv, env);
			}
			return pid;

		} else {
			this->createResponse();
		}

	} else { //dir
		
		//location block 확인

			//정확히 일치하는 block이면
				// index 키에 해당하는 value있으면 보여줌  (1)
				// index.html 있으면 보여줌				(2)
				// autoindex yes => 리스트 보여줌		(3)
				// autoindex np => 빈페이지 보여줌		(4)
			//안일치 블록 && 존재하는 dir이면
				// index.html 있으면 보여줌			(1)
				// autoindex yes => 리스트 보여줌	(2)
				// autoindex np => 빈페이지 보여줌	(3)
			//존재 안하는 dir이면
				//400 error						(1)

	}

	return 0;
}

const string	RGet::createResponse()
{
	string	mMSG;
	char 	timeStamp[TIME_SIZE];
	stringstream to_str;
	stringstream tmp;
	string		buffer;
	string		body;

	//요청받은 파일 크기 계산 및 BODY용 stream 처리 (default:index.htmp)
	ifstream	fin(getRoot());
	if (fin.fail())
		throw runtime_error("Error: GET body file read failed");
	tmp << fin.rdbuf();
	body = tmp.str();
	fin.close();

	//1st line: STATUS
	mMSG.append(HTTP_STATUS);	//"HTTP/1.1 200 OK\r\n"
	
	//HEADER============================================
	Time::stamp(timeStamp);
	mMSG.append(timeStamp);		//Date: Tue, 20 Jul 2023 12:34:56 GMT\r\n
	mMSG.append(SPIDER_SERVER);	//Server: SpiderMen/1.0.0\r\n

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
		mMSG.append("Connection: Keep-Alive\r\n");

	mMSG.append("\r\n"); //end of head

	//BODY 추가
	mMSG.append(body.c_str(), body.size());
	return mMSG;
}
